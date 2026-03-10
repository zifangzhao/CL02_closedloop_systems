#include "CE32_TTL_Trigger.h"
#include "CE32_macro.h"

/* ─── Initialization ─────────────────────────────────────────── */

void CE32_TTL_Init(CE32_TTL_Config* cfg,
                   GPIO_TypeDef* port, uint16_t pin,
                   IRQn_Type irqn,
                   CE32_stimulator* stim)
{
    // Populate config struct
    cfg->port = port;
    cfg->pin  = pin;
    cfg->irqn = irqn;
    cfg->stim = stim;
    cfg->state = 0;
    cfg->priority = CE32_TTL_DEFAULT_PRIORITY;
    cfg->last_edge_ms = 0;

    /* Configure GPIO as external interrupt, rising + falling edge */
    GPIO_InitTypeDef gpio = {0};  // aggregate init: set the first member to 0, & 0-init everything else
    gpio.Pin  = pin;
    gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio.Pull = GPIO_PULLUP;  /* Match CubeMX config — pull-up when nothing connected */
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(port, &gpio);

    /* Set NVIC priority but don't enable yet */
    HAL_NVIC_SetPriority(irqn, cfg->priority, 0);
}

/* ─── Enable / Disable ───────────────────────────────────────── */

void CE32_TTL_Enable(CE32_TTL_Config* cfg)
{
    cfg->state |= CE32_TTL_STATE_ENABLED;  // if already enabled, this is a no-op
    cfg->last_edge_ms = HAL_GetTick();  // reset debounce timer

    /* Clear any pending EXTI flag before enabling */
    __HAL_GPIO_EXTI_CLEAR_IT(cfg->pin);
    HAL_NVIC_EnableIRQ(cfg->irqn);
}

void CE32_TTL_Disable(CE32_TTL_Config* cfg)
{
    cfg->state &= ~(CE32_TTL_STATE_ENABLED | CE32_TTL_STATE_ACTIVE);  // clear both flags
    HAL_NVIC_DisableIRQ(cfg->irqn);

    /* Abort any ongoing TTL-initiated stimulation */
    CE32_STIM_Abort(cfg->stim);
}

/* ─── Priority ───────────────────────────────────────────────── */

void CE32_TTL_SetPriority(CE32_TTL_Config* cfg, uint8_t priority)
{
    cfg->priority = priority;
    HAL_NVIC_SetPriority(cfg->irqn, priority, 0);
}

/* ─── IRQ Handler (called from EXTI3_IRQHandler) ──────────────── */

void CE32_TTL_IRQHandler(CE32_TTL_Config* cfg)
{
    /* Only process if enabled */
    if ((cfg->state & CE32_TTL_STATE_ENABLED) == 0)
        return;

    /* Debounce: ignore edges too close together */
    uint32_t now = HAL_GetTick();
    if ((now - cfg->last_edge_ms) < CE32_TTL_DEBOUNCE_MS)
        return;
    cfg->last_edge_ms = now;

    /* Read current pin level */
    GPIO_PinState level = HAL_GPIO_ReadPin(cfg->port, cfg->pin);

    if (level == GPIO_PIN_SET)
    {
        /* TTL went HIGH (rising edge) → trigger stimulation */
        cfg->state |= CE32_TTL_STATE_ACTIVE;
        CE32_STIM_Trig(cfg->stim);
    }
    else
    {
        /* TTL went LOW (falling edge) → abort stimulation */
        cfg->state &= ~CE32_TTL_STATE_ACTIVE;
        CE32_STIM_Abort(cfg->stim);
    }
}

/* ─── State Query ────────────────────────────────────────────── */

int CE32_TTL_GetState(CE32_TTL_Config* cfg)
{
    /* Active-high: pin HIGH = TTL active (behavioral controller asserting) */
    return HAL_GPIO_ReadPin(cfg->port, cfg->pin) == GPIO_PIN_SET;
}
