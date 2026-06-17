---
hide:
  - navigation
  - toc
---

<section class="cl02-hero">
  <div>
    <h1>A compact closed-loop controller for LFP-triggered stimulation</h1>
    <p>CL02 detects configured local-field-potential oscillation features from analog inputs and drives timed trigger outputs for closed-loop electrophysiology experiments.</p>
    <div class="cl02-actions">
      <a class="md-button md-button--primary" href="getting-started/">Get Started</a>
      <a class="md-button" href="hardware/">Hardware</a>
      <a class="md-button" href="software/">Software</a>
      <a class="md-button" href="firmware-analysis/">Firmware</a>
      <a class="md-button" href="https://github.com/zifangzhao/CL02_closedloop_systems">GitHub</a>
    </div>
    <div class="cl02-signal-row">
      <span>IN0 and IN1 analog inputs</span>
      <span>DOUT trigger output</span>
      <span>AOUT analog output</span>
      <span>1 kHz DSP workflow</span>
      <span>Windows control center</span>
    </div>
  </div>
  <div class="cl02-hero-media cl02-hero-media-tiled">
    <figure class="cl02-hero-tile cl02-device-tile">
      <img src="images/cl02-front-panel.png" alt="CL02 front panel with IN0, IN1, DOUT, AOUT, and status indicators">
      <figcaption class="cl02-caption">The CL02 front panel exposes two analog inputs, digital and analog outputs, and three state indicators for closed-loop readiness and trigger state.</figcaption>
    </figure>
    <figure class="cl02-hero-tile cl02-ui-tile">
      <img src="images/cl02-control-center.png" alt="CL02 Windows control center showing waveforms and trigger settings">
    </figure>
  </div>
</section>

<section class="cl02-section">
  <div class="cl02-grid two">
    <div class="cl02-card">
      <h2>Current Public Workflow</h2>
      <p><strong>Stable public scope:</strong> connect the CL02 device over USB, configure filters and trigger timing in the Windows control center, and use DOUT or AOUT to drive downstream stimulation hardware.</p>
    </div>
    <div class="cl02-card">
      <h2>Release Record</h2>
      <p>Record the installer file, firmware image, PCB revision, filter file, stimulus timing parameters, and threshold settings with each experiment so trigger behavior remains reproducible.</p>
    </div>
  </div>

  <div class="cl02-grid">
    <div class="cl02-card">
      <h3>Run a first bench test</h3>
      <p><a href="getting-started/installation/">Install</a> -> <a href="getting-started/first-run/">connect</a> -> configure a conservative trigger -> verify DOUT timing.</p>
    </div>
    <div class="cl02-card">
      <h3>Prepare hardware</h3>
      <p><a href="hardware/connections-indicators/">Connection map</a> -> status indicators -> stimulator input checks -> PCB revision notes.</p>
    </div>
    <div class="cl02-card">
      <h3>Tune detection</h3>
      <p><a href="software/filters/">Filter choice</a> -> moving-average order -> baseline training window -> threshold or trigger-level setting.</p>
    </div>
    <div class="cl02-card">
      <h3>Update firmware</h3>
      <p><a href="getting-started/firmware-update/">DFU mode</a> -> choose a repository firmware image -> upgrade -> return the switch to Run mode.</p>
    </div>
  </div>
</section>

<section class="cl02-section">
  <h2>Closed-Loop Signal Path</h2>
  <p>CL02 routes analog input through configured filtering and trigger control before asserting the stimulation output.</p>
  <div class="cl02-flow" aria-label="CL02 signal path">
    <div>Analog input</div>
    <div>DSP filter</div>
    <div>Envelope or metric</div>
    <div>Baseline training</div>
    <div>Threshold test</div>
    <div>DOUT or AOUT</div>
  </div>
</section>

<section class="cl02-section">
  <h2>Interface and I/O Summary</h2>
  <div class="cl02-spec-scroll" role="region" aria-label="CL02 input output summary" tabindex="0">
    <table class="cl02-spec-table">
      <thead>
        <tr>
          <th>Signal or control</th>
          <th>Public behavior</th>
          <th>Documentation entry</th>
        </tr>
      </thead>
      <tbody>
        <tr><td>IN0</td><td>Analog input 0, 0-3.3 V, configurable with +1.5 V offset.</td><td><a href="hardware/connections-indicators/">Connections</a></td></tr>
        <tr><td>IN1</td><td>Analog input 1, 0-3.3 V, configurable with +1.5 V offset.</td><td><a href="hardware/connections-indicators/">Connections</a></td></tr>
        <tr><td>DOUT</td><td>Digital trigger output, 0-3.3 V.</td><td><a href="software/trigger-logic/">Trigger Logic</a></td></tr>
        <tr><td>AOUT</td><td>Analog output, 0-3.3 V.</td><td><a href="hardware/connections-indicators/">Connections</a></td></tr>
        <tr><td>Ready</td><td>Closed-loop processor state: slow blink while waiting, fast blink while estimating baseline, on when ready.</td><td><a href="hardware/connections-indicators/">Indicators</a></td></tr>
        <tr><td>Detect</td><td>Indicates that a trigger condition is armed; DOUT behavior still depends on stimulator settings.</td><td><a href="hardware/connections-indicators/">Indicators</a></td></tr>
        <tr><td>Trigger</td><td>Indicates DOUT state.</td><td><a href="hardware/connections-indicators/">Indicators</a></td></tr>
      </tbody>
    </table>
  </div>
</section>

<section class="cl02-section">
  <h2>Operator Controls</h2>
  <p>The Windows control center presents waveform preview, serial connection, trigger timing, DSP filter selection, and display scaling in one bench-facing interface.</p>
  <div class="cl02-grid">
    <div class="cl02-card">
      <h3>Connection</h3>
      <p>Choose the COM port, connect to the device, and optionally log data to a file during preview.</p>
    </div>
    <div class="cl02-card">
      <h3>Trigger timing</h3>
      <p>Set interval, pulse width, pulse cycles, fixed delay, random delay, and training delay or duration.</p>
    </div>
    <div class="cl02-card">
      <h3>DSP configuration</h3>
      <p>Select filter, formula, moving-average order, trigger threshold, trigger level, and optional custom filters.</p>
    </div>
    <div class="cl02-card">
      <h3>Manual checks</h3>
      <p>Restart DSP or force a trigger during bench validation before moving to experiment hardware.</p>
    </div>
    <div class="cl02-card">
      <h3>External override</h3>
      <p>Read an int32 control value from <code>d:\cl02_control.bin</code> to mask DOUT in the experimental override workflow.</p>
    </div>
    <div class="cl02-card">
      <h3>Display scaling</h3>
      <p>Select display time, input gain, DSP gain, DAC gain, and optional DC removal for waveform review.</p>
    </div>
  </div>
</section>

<section class="cl02-section">
  <h2>System Overview</h2>
  <p>A typical session starts with a bench connection, then moves through filter selection, baseline training, trigger verification, and experiment logging.</p>
  <figure class="cl02-image-frame">
    <img src="images/cl02-control-center.png" alt="CL02 Windows control center with waveform and closed-loop settings">
  </figure>
</section>

<section class="cl02-section">
  <h2>Quick Start</h2>
  <div class="cl02-grid two">
    <div class="cl02-card">
      <h3>Install software</h3>
      <p>Use the installer under <code>Windows</code> and install the CL02 USB driver manually if Windows does not bind a usable COM port.</p>
    </div>
    <div class="cl02-card">
      <h3>Connect hardware</h3>
      <p>Wire IN0 or IN1, connect DOUT or AOUT to the downstream device, and confirm voltage compatibility before enabling triggers.</p>
    </div>
    <div class="cl02-card">
      <h3>Train and trigger</h3>
      <p>Select filter, moving-average order, training window, threshold, interval, pulse width, and delay before enabling output.</p>
    </div>
    <div class="cl02-card">
      <h3>Save a record</h3>
      <p>Log the installer version, firmware file, trigger settings, filter file, and hardware connections used for the run.</p>
    </div>
  </div>
</section>
