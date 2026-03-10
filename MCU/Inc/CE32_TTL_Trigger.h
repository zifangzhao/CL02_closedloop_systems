#ifndef __CE32_TTL_TRIGGER  // include a section of code iff __CE32_TTL_TRIGGER is not defined
#define __CE32_TTL_TRIGGER

#include "stm32f3xx_hal.h"
#include "CE32_stimulator.h"

/* ─── TTL Trigger Configuration ─────────────────────────────── */

/* Default Nested Vectored Interrupt Controller (NVIC) preemption priority for the
   TTL EXTI interrupt. Lower number == higher priority. Direct Memory Access (DMA)
   is at 0, so 1 makes TTL the highest non-DMA interrupt. */
#define CE32_TTL_DEFAULT_PRIORITY  1

// Debouncing = ignoring edges that happen too close together
/* Minimum time between edges (in SysTick ms) to debounce */
#define CE32_TTL_DEBOUNCE_MS       1

/* State flags */
#define CE32_TTL_STATE_ENABLED     0x0001U
#define CE32_TTL_STATE_ACTIVE      0x0002U  /* TTL is active (pin HIGH, behavioral controller asserting) */

typedef struct {
    GPIO_TypeDef*       port;          /* GPIO port (e.g. GPIOB) */
    uint16_t            pin;           /* GPIO pin mask (e.g. GPIO_PIN_3) */
    IRQn_Type           irqn;         /* EXTI (External Interrupt) IRQ (Interrupt Request) number */
    uint16_t            state;         /* TTL module state flags */
    uint8_t             priority;      /* NVIC preemption priority */
    uint32_t            last_edge_ms;  /* Timestamp of last edge (for debounce) */
    CE32_stimulator*    stim;          /* Pointer to the stimulator to control */
} CE32_TTL_Config;

/* ─── API ────────────────────────────────────────────────────── */

/**
 * @brief Initialize the TTL trigger module.
 *        Configures the GPIO pin as EXTI (rising + falling),
 *        sets NVIC priority, but does NOT enable the interrupt yet.
 *        Call CE32_TTL_Enable() to start listening.
 *
 * @param cfg     Pointer to TTL config struct (caller-owned)
 * @param port    GPIO port for the TTL input pin
 * @param pin     GPIO pin mask
 * @param irqn    NVIC IRQ number for the EXTI line
 * @param stim    Pointer to the CE32_stimulator to trigger/abort
 */
void CE32_TTL_Init(CE32_TTL_Config* cfg,
                   GPIO_TypeDef* port, uint16_t pin,
                   IRQn_Type irqn,
                   CE32_stimulator* stim);

/**
 * @brief Enable TTL interrupt listening.
 */
void CE32_TTL_Enable(CE32_TTL_Config* cfg);

/**
 * @brief Disable TTL interrupt listening.
 *        Also aborts any ongoing stimulation triggered by TTL.
 */
void CE32_TTL_Disable(CE32_TTL_Config* cfg);

/**
 * @brief Set NVIC preemption priority for the TTL interrupt.
 *        Can be called at runtime to adjust priority hierarchy.
 */
void CE32_TTL_SetPriority(CE32_TTL_Config* cfg, uint8_t priority);

/**
 * @brief Called from the EXTI ISR (Interrupt Service Routine). Reads pin state and
 *        triggers or aborts stimulation accordingly.
 *        Includes debounce protection.
 */
void CE32_TTL_IRQHandler(CE32_TTL_Config* cfg);

/**
 * @brief Returns 1 if TTL is active (pin HIGH, behavioral controller asserting), 0 if idle.
 */
int CE32_TTL_GetState(CE32_TTL_Config* cfg);

#endif /* __CE32_TTL_TRIGGER */
