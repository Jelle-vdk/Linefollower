#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="nl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LineFollower Jelle</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: #121212;
            color: #eee;
            margin: 0;
            padding: 20px;
        }
        h1 {
            text-align: center;
            margin-top: 0;
            margin-bottom: 25px;
            color: #fff;
        }
        .page-layout {
            display: grid;
            grid-template-columns: 1fr minmax(320px, 600px) 1fr;
            gap: 25px;
            align-items: start;
            max-width: 1400px;
            margin: 0 auto;
        }
        .main-column {
            grid-column: 2;
            width: 100%;
        }
        .sidebar-column {
            grid-column: 3;
            justify-self: end;
            width: 100%;
            max-width: 280px;
        }
        @media (max-width: 980px) {
            .page-layout {
                display: flex;
                flex-direction: column;
                max-width: 600px;
            }
            .sidebar-column {
                max-width: 100%;
            }
        }
        .section {
            background: #1e1e1e;
            padding: 20px;
            border-radius: 8px;
            margin-bottom: 20px;
            border: 1px solid #333;
            box-sizing: border-box;
        }
        .section-title {
            font-size: 1.1em;
            font-weight: bold;
            margin-bottom: 12px;
            color: #bbb;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .status-container {
            display: flex;
            gap: 10px;
        }
        .status-box {
            flex: 1;
            text-align: center;
            padding: 15px 10px;
            background: #2a2a2a;
            border: 2px solid #3a3a3a;
            border-radius: 6px;
            font-weight: bold;
            color: #777;
            cursor: pointer;
            user-select: none;
            transition: all 0.2s ease;
        }
        .status-box:hover {
            border-color: #666;
            color: #bbb;
        }
        .status-box.active-stop {
            background: #b71c1c;
            border-color: #f44336;
            color: #fff;
        }
        .status-box.active-run {
            background: #1b5e20;
            border-color: #4caf50;
            color: #fff;
        }
        .status-box.active-manual {
            background: #e65100;
            border-color: #ff9800;
            color: #fff;
        }

        /* RUN parameter boxen */
        .param-list {
            display: flex;
            flex-direction: column;
            gap: 10px;
        }
        .param-row {
            display: flex;
            align-items: center;
            gap: 10px;
            background: #121212;
            border: 1px solid #333;
            border-radius: 6px;
            padding: 10px 12px;
        }
        .param-label {
            min-width: 65px;
            font-weight: bold;
            font-family: monospace;
            font-size: 14px;
            color: #ddd;
        }
        .param-input {
            flex: 1;
            min-width: 0;
            padding: 8px 10px;
            background: #1e1e1e;
            border: 1px solid #444;
            border-radius: 5px;
            color: #fff;
            font-family: monospace;
            font-size: 14px;
        }
        .param-input:focus {
            outline: none;
            border-color: #4caf50;
        }
        .param-send-btn {
            padding: 8px 14px;
            background: #2e7d32;
            color: #fff;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            cursor: pointer;
        }
        .param-send-btn:hover {
            background: #388e3c;
        }
        .param-current {
            min-width: 64px;
            text-align: center;
            font-family: monospace;
            font-size: 13px;
            font-weight: bold;
            color: #00e676;
            background: #1e1e1e;
            padding: 7px 8px;
            border-radius: 4px;
            border: 1px solid #333;
        }

        #sensorCanvas {
            width: 100%;
            height: 210px;
            background: #121212;
            border: 1px solid #333;
            border-radius: 5px;
            display: block;
        }
        .pos-badge {
            font-family: monospace;
            font-size: 0.9em;
            color: #00e676;
            background: #121212;
            padding: 3px 8px;
            border-radius: 4px;
            border: 1px solid #333;
        }
        .drive-layout {
            display: flex;
            gap: 12px;
            align-items: stretch;
            margin-bottom: 16px;
        }
        .drive-center-stack {
            flex: 1.4;
            display: flex;
            flex-direction: column;
            gap: 12px;
        }
        .drive-btn {
            background: #2a2a2a;
            border: 2px solid #3e3e3e;
            border-radius: 8px;
            color: #eee;
            font-weight: bold;
            font-size: 14px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            text-align: center;
            cursor: pointer;
            user-select: none;
            -webkit-user-select: none;
            touch-action: none;
            transition: background 0.1s, border-color 0.1s, transform 0.05s;
        }
        .drive-btn:hover {
            border-color: #666;
        }
        .drive-btn.pressing {
            background: #e65100;
            border-color: #ff9800;
            color: #fff;
            transform: scale(0.98);
        }
        .btn-horiz {
            height: 58px;
            padding: 0 14px;
        }
        .btn-vert {
            flex: 0.8;
            min-height: 128px;
            padding: 10px 6px;
            line-height: 1.3;
        }
        .speed-control {
            background: #121212;
            border: 1px solid #333;
            border-radius: 6px;
            padding: 12px 14px;
        }
        .speed-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 8px;
            font-size: 13px;
            color: #bbb;
            font-weight: bold;
        }
        input[type="range"] {
            width: 100%;
            accent-color: #ff9800;
            cursor: pointer;
        }
        .cmd-bar {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
        }
        input[type="text"] {
            flex: 1;
            padding: 10px 12px;
            background: #121212;
            border: 1px solid #444;
            border-radius: 5px;
            color: #fff;
            font-family: monospace;
            font-size: 14px;
        }
        input[type="text"]:focus {
            outline: none;
            border-color: #4caf50;
        }
        button {
            padding: 10px 18px;
            background: #2e7d32;
            color: white;
            border: none;
            border-radius: 5px;
            font-weight: bold;
            cursor: pointer;
        }
        button:hover {
            background: #388e3c;
        }
        #serialMonitor {
            width: 100%;
            height: 250px;
            background: #0a0a0a;
            color: #00e676;
            font-family: monospace;
            font-size: 13px;
            padding: 12px;
            border: 1px solid #333;
            border-radius: 5px;
            overflow-y: auto;
            box-sizing: border-box;
            white-space: pre-wrap;
        }
        .cmd-list {
            display: flex;
            flex-direction: column;
            gap: 8px;
        }
        .cmd-item {
            background: #121212;
            border: 1px solid #333;
            border-radius: 5px;
            padding: 8px 10px;
            cursor: pointer;
            transition: border-color 0.2s, background 0.2s;
        }
        .cmd-item:hover {
            border-color: #4caf50;
            background: #181818;
        }
        .cmd-code {
            font-family: monospace;
            color: #00e676;
            font-size: 13px;
            font-weight: bold;
            display: block;
        }
        .cmd-desc {
            font-size: 12px;
            color: #888;
            margin-top: 2px;
            display: block;
        }
    </style>
</head>
<body>

    <h1>LineFollower Jelle</h1>

    <div class="page-layout">
        <div class="main-column">
            <div class="section">
                <div class="section-title"><span>Status:</span></div>
                <div class="status-container">
                    <div id="box-stop" class="status-box active-stop" onclick="sendQuickCmd('set state stop')">STOP</div>
                    <div id="box-run" class="status-box" onclick="sendQuickCmd('set state run')">RUN</div>
                    <div id="box-manual" class="status-box" onclick="sendQuickCmd('set state manual')">MANUAL</div>
                </div>
            </div>

            <!-- RUN parameters tussen Status en Terminal -->
            <div id="runParamsSection" class="section" style="display: none;">
                <div class="section-title"><span>PID &amp; Motor Parameters:</span></div>
                <div class="param-list">
                    <div class="param-row">
                        <span class="param-label">power</span>
                        <input type="number" step="any" id="input-power" class="param-input" placeholder="0 - 255 (bv. 100)" onkeypress="if(event.key==='Enter') sendParam('power')">
                        <button class="param-send-btn" onclick="sendParam('power')">Send</button>
                        <span id="val-power" class="param-current">--</span>
                    </div>
                    <div class="param-row">
                        <span class="param-label">diff</span>
                        <input type="number" step="any" id="input-diff" class="param-input" placeholder="0 - 1 (bv. 0.5)" onkeypress="if(event.key==='Enter') sendParam('diff')">
                        <button class="param-send-btn" onclick="sendParam('diff')">Send</button>
                        <span id="val-diff" class="param-current">--</span>
                    </div>
                    <div class="param-row">
                        <span class="param-label">Kp</span>
                        <input type="number" step="any" id="input-Kp" class="param-input" placeholder="bv. 7.0" onkeypress="if(event.key==='Enter') sendParam('Kp')">
                        <button class="param-send-btn" onclick="sendParam('Kp')">Send</button>
                        <span id="val-Kp" class="param-current">--</span>
                    </div>
                    <div class="param-row">
                        <span class="param-label">Ki</span>
                        <input type="number" step="any" id="input-Ki" class="param-input" placeholder="in s (bv. 0.5)" onkeypress="if(event.key==='Enter') sendParam('Ki')">
                        <button class="param-send-btn" onclick="sendParam('Ki')">Send</button>
                        <span id="val-Ki" class="param-current">--</span>
                    </div>
                    <div class="param-row">
                        <span class="param-label">Kd</span>
                        <input type="number" step="any" id="input-Kd" class="param-input" placeholder="in s (bv. 0.05)" onkeypress="if(event.key==='Enter') sendParam('Kd')">
                        <button class="param-send-btn" onclick="sendParam('Kd')">Send</button>
                        <span id="val-Kd" class="param-current">--</span>
                    </div>
                </div>
            </div>

            <div id="sensorSection" class="section" style="display: none;">
                <div class="section-title">
                    <span>Sensoren &amp; Lijnpositie:</span>
                    <span id="posLabel" class="pos-badge">Positie: 0.00 mm</span>
                </div>
                <canvas id="sensorCanvas" width="560" height="210"></canvas>
            </div>

            <div id="manualDriveSection" class="section" style="display: none;">
                <div class="section-title"><span>Manuele Besturing:</span></div>
                <div class="drive-layout">
                    <div class="drive-btn btn-vert" data-dir="left">&#9664;<br>Links<br>draaien</div>
                    <div class="drive-center-stack">
                        <div class="drive-btn btn-horiz" data-dir="forward">&#9650; Vooruit</div>
                        <div class="drive-btn btn-horiz" data-dir="backward">&#9660; Achteruit</div>
                    </div>
                    <div class="drive-btn btn-vert" data-dir="right">&#9654;<br>Rechts<br>draaien</div>
                </div>
                <div class="speed-control">
                    <div class="speed-header">
                        <span>Snelheid:</span>
                        <span id="speedValue" class="pos-badge">60%</span>
                    </div>
                    <input type="range" id="speedSlider" min="0" max="100" value="60" oninput="updateSpeedLabel(this.value)">
                </div>
            </div>

            <div class="section">
                <div class="section-title"><span>Terminal:</span></div>
                <div class="cmd-bar">
                    <input type="text" id="cmdInput" placeholder="Typ een commando (bv. set state run)..." onkeypress="if(event.key==='Enter') sendCmd()">
                    <button onclick="sendCmd()">Stuur</button>
                </div>
                <div id="serialMonitor">Verbinden met robot...</div>
            </div>
        </div>

        <div class="sidebar-column">
            <div class="section">
                <div class="section-title"><span>Commando's:</span></div>
                <div class="cmd-list">
                    <div class="cmd-item" onclick="sendQuickCmd('debug')">
                        <span class="cmd-code">debug</span>
                        <span class="cmd-desc">Toon cyclustijd &amp; rekentijd</span>
                    </div>
                    <div class="cmd-item" onclick="fillCmd('set cycle ')">
                        <span class="cmd-code">set cycle &lt;us&gt;</span>
                        <span class="cmd-desc">Stel cyclustijd in (µs)</span>
                    </div>
                    <div class="cmd-item" onclick="fillCmd('set power ')">
                        <span class="cmd-code">set power &lt;val&gt;</span>
                        <span class="cmd-desc">Basissnelheid motoren (0-255)</span>
                    </div>
                    <div class="cmd-item" onclick="fillCmd('set diff ')">
                        <span class="cmd-code">set diff &lt;val&gt;</span>
                        <span class="cmd-desc">Snelheidsverdeling bochten (0-1)</span>
                    </div>
                    <div class="cmd-item" onclick="fillCmd('set Kp ')">
                        <span class="cmd-code">set Kp &lt;val&gt;</span>
                        <span class="cmd-desc">Proportionele versterking</span>
                    </div>
                    <div class="cmd-item" onclick="fillCmd('set Ki ')">
                        <span class="cmd-code">set Ki &lt;val&gt;</span>
                        <span class="cmd-desc">Integrerende actie (s)</span>
                    </div>
                    <div class="cmd-item" onclick="fillCmd('set Kd ')">
                        <span class="cmd-code">set Kd &lt;val&gt;</span>
                        <span class="cmd-desc">Differentiërende actie (s)</span>
                    </div>
                    <div class="cmd-item" onclick="sendQuickCmd('set state run')">
                        <span class="cmd-code">set state run</span>
                        <span class="cmd-desc">Start linefollower cyclus</span>
                    </div>
                    <div class="cmd-item" onclick="sendQuickCmd('set state stop')">
                        <span class="cmd-code">set state stop</span>
                        <span class="cmd-desc">Stop de robot</span>
                    </div>
                    <div class="cmd-item" onclick="sendQuickCmd('set state manual')">
                        <span class="cmd-code">set state manual</span>
                        <span class="cmd-desc">Zet in manuele modus</span>
                    </div>
                    <div class="cmd-item" onclick="sendQuickCmd('calibrate black')">
                        <span class="cmd-code">calibrate black</span>
                        <span class="cmd-desc">Kalibreer zwarte lijnwaarden</span>
                    </div>
                    <div class="cmd-item" onclick="sendQuickCmd('calibrate white')">
                        <span class="cmd-code">calibrate white</span>
                        <span class="cmd-desc">Kalibreer witte ondergrondwaarden</span>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <script>
        let gateway = `ws://${window.location.hostname}/ws`;
        let websocket;
        const SENSOR_PITCH_MM = 9.525;
        let activeDir = null;

        window.addEventListener('load', () => {
            drawSensorGraph([0, 0, 0, 0, 0, 0], 0);
            initDriveButtons();
            initWebSocket();
        });

        function initWebSocket() {
            websocket = new WebSocket(gateway);
            websocket.onopen = onOpen;
            websocket.onclose = onClose;
            websocket.onmessage = onMessage;
        }

        function onOpen(event) {
            document.getElementById('serialMonitor').textContent = 'Verbonden met LineFollower!';
        }

        function onClose(event) {
            activeDir = null;
            addSerialLine('[Verbinding verbroken, opnieuw proberen...]');
            setTimeout(initWebSocket, 2000);
        }

        function onMessage(event) {
            const msg = event.data;
            if (msg.startsWith('STATE:')) {
                const state = msg.substring(6).trim().toLowerCase();
                updateStatusUI(state);
            } else if (msg.startsWith('PARAMS:')) {
                const parts = msg.substring(7).split('|');
                if (parts[0] !== undefined) document.getElementById('val-power').textContent = parts[0];
                if (parts[1] !== undefined) document.getElementById('val-diff').textContent = parts[1];
                if (parts[2] !== undefined) document.getElementById('val-Kp').textContent = parts[2];
                if (parts[3] !== undefined) document.getElementById('val-Ki').textContent = parts[3];
                if (parts[4] !== undefined) document.getElementById('val-Kd').textContent = parts[4];
            } else if (msg.startsWith('SENSORS:')) {
                const parts = msg.substring(8).split('|');
                const vals = parts[0].split(',').map(Number);
                const posMm = parseFloat(parts[1]) || 0;
                document.getElementById('posLabel').textContent = 'Positie: ' + posMm.toFixed(2) + ' mm';
                drawSensorGraph(vals, posMm);
            } else if (msg === 'CLEAR') {
                document.getElementById('serialMonitor').textContent = '';
            } else {
                if (msg.startsWith('power updated to: ') || msg.startsWith('power: ')) {
                    document.getElementById('val-power').textContent = msg.split(': ')[1].trim();
                } else if (msg.startsWith('diff updated to: ') || msg.startsWith('diff: ')) {
                    document.getElementById('val-diff').textContent = msg.split(': ')[1].trim();
                } else if (msg.startsWith('Kp updated to: ') || msg.startsWith('Kp: ')) {
                    document.getElementById('val-Kp').textContent = msg.split(': ')[1].trim();
                } else if (msg.startsWith('Ki updated to: ') || msg.startsWith('Ki: ')) {
                    document.getElementById('val-Ki').textContent = msg.split(': ')[1].trim();
                } else if (msg.startsWith('Kd updated to: ') || msg.startsWith('Kd: ')) {
                    document.getElementById('val-Kd').textContent = msg.split(': ')[1].trim();
                }
                addSerialLine(msg);
            }
        }

        function sendParam(name) {
            const input = document.getElementById('input-' + name);
            const val = input.value.trim();
            if (val === '') return;

            sendQuickCmd('set ' + name + ' ' + val);
            input.value = '';
        }

        function pctToPwm(pct) {
            return Math.round((pct / 100) * 255);
        }

        function updateSpeedLabel(val) {
            document.getElementById('speedValue').textContent = val + '%';
            if (activeDir) {
                sendSilentCmd('drive ' + activeDir + ' ' + pctToPwm(val));
            }
        }

        function initDriveButtons() {
            const buttons = document.querySelectorAll('.drive-btn');
            buttons.forEach(btn => {
                const dir = btn.getAttribute('data-dir');

                const startDrive = (e) => {
                    e.preventDefault();
                    if (btn.setPointerCapture && e.pointerId !== undefined) {
                        btn.setPointerCapture(e.pointerId);
                    }
                    btn.classList.add('pressing');
                    activeDir = dir;
                    const pct = document.getElementById('speedSlider').value;
                    sendSilentCmd('drive ' + dir + ' ' + pctToPwm(pct));
                };

                const stopDrive = (e) => {
                    if (!btn.classList.contains('pressing')) return;
                    btn.classList.remove('pressing');
                    activeDir = null;
                    sendSilentCmd('drive stop');
                };

                btn.addEventListener('pointerdown', startDrive);
                btn.addEventListener('pointerup', stopDrive);
                btn.addEventListener('pointercancel', stopDrive);
                btn.addEventListener('lostpointercapture', stopDrive);
            });

            window.addEventListener('blur', () => {
                if (activeDir) {
                    activeDir = null;
                    document.querySelectorAll('.drive-btn').forEach(b => b.classList.remove('pressing'));
                    sendSilentCmd('drive stop');
                }
            });
        }

        function sendSilentCmd(cmd) {
            if (websocket && websocket.readyState === WebSocket.OPEN) {
                websocket.send(cmd);
            }
        }

        function drawSensorGraph(vals, posMm) {
            const canvas = document.getElementById('sensorCanvas');
            const ctx = canvas.getContext('2d');
            const w = canvas.width;
            const h = canvas.height;
            const padL = 52, padR = 20, padT = 20, padB = 30;
            const plotW = w - padL - padR;
            const plotH = h - padT - padB;

            ctx.clearRect(0, 0, w, h);

            ctx.strokeStyle = '#222';
            ctx.lineWidth = 1;
            for (let g = 1; g <= 4; g++) {
                const gy = padT + plotH - (g * 0.25) * plotH;
                ctx.beginPath();
                ctx.moveTo(padL, gy);
                ctx.lineTo(padL + plotW, gy);
                ctx.stroke();
            }

            ctx.strokeStyle = '#666';
            ctx.lineWidth = 1.5;
            ctx.beginPath();
            ctx.moveTo(padL, padT);
            ctx.lineTo(padL, padT + plotH);
            ctx.lineTo(padL + plotW, padT + plotH);
            ctx.stroke();

            ctx.fillStyle = '#888';
            ctx.font = '11px monospace';
            ctx.fillText('1000', 10, padT + 6);
            ctx.fillText('zwart', 10, padT + 18);
            ctx.fillText('500', 16, padT + plotH / 2 + 4);
            ctx.fillText('0 wit', 10, padT + plotH);

            const step = plotW / 6;
            const xs = [];
            for (let i = 0; i < 6; i++) {
                xs.push(padL + step * (i + 0.5));
            }

            const centerX = padL + plotW / 2;
            ctx.save();
            ctx.setLineDash([4, 4]);
            ctx.strokeStyle = '#444';
            ctx.beginPath();
            ctx.moveTo(centerX, padT);
            ctx.lineTo(centerX, padT + plotH);
            ctx.stroke();
            ctx.restore();

            const barW = 24;
            for (let i = 0; i < 6; i++) {
                const v = Math.max(0, Math.min(1000, vals[i] || 0));
                const barH = (v / 1000) * plotH;
                const x = xs[i] - barW / 2;
                const y = padT + plotH - barH;

                ctx.fillStyle = 'rgba(0, 230, 118, 0.22)';
                ctx.strokeStyle = '#00e676';
                ctx.lineWidth = 1.5;
                ctx.fillRect(x, y, barW, barH);
                ctx.strokeRect(x, y, barW, barH);

                ctx.fillStyle = '#bbb';
                ctx.fillText('S' + (i + 1), xs[i] - 7, padT + plotH + 18);
            }

            const sensorIdxPos = (posMm / SENSOR_PITCH_MM) + 2.5;
            const clampedIdx = Math.max(-0.3, Math.min(5.3, sensorIdxPos));
            const estX = padL + step * (clampedIdx + 0.5);
            const maxVal = Math.max(...vals);

            const peakY = padT + plotH - (Math.min(1000, maxVal * 1.05) / 1000) * plotH;
            let pts = [{ x: padL, y: padT + plotH }];
            for (let i = 0; i < 6; i++) {
                pts.push({ x: xs[i], y: padT + plotH - ((vals[i] || 0) / 1000) * plotH });
            }
            if (maxVal > 120) {
                pts.push({ x: estX, y: peakY });
            }
            pts.push({ x: padL + plotW, y: padT + plotH });
            pts.sort((a, b) => a.x - b.x);

            ctx.strokeStyle = '#ff5252';
            ctx.lineWidth = 2.5;
            ctx.beginPath();
            ctx.moveTo(pts[0].x, pts[0].y);
            for (let i = 0; i < pts.length - 1; i++) {
                const xc = (pts[i].x + pts[i + 1].x) / 2;
                const yc = (pts[i].y + pts[i + 1].y) / 2;
                ctx.quadraticCurveTo(pts[i].x, pts[i].y, xc, yc);
            }
            ctx.lineTo(pts[pts.length - 1].x, pts[pts.length - 1].y);
            ctx.stroke();

            if (maxVal > 120) {
                ctx.fillStyle = '#050505';
                ctx.strokeStyle = '#ffffff';
                ctx.lineWidth = 1.5;
                ctx.fillRect(estX - 4, padT, 8, plotH);
                ctx.strokeRect(estX - 4, padT, 8, plotH);
            }
        }

        function updateStatusUI(state) {
            document.getElementById('box-stop').className = 'status-box';
            document.getElementById('box-run').className = 'status-box';
            document.getElementById('box-manual').className = 'status-box';

            const runParamsSection = document.getElementById('runParamsSection');
            const sensorSection = document.getElementById('sensorSection');
            const manualDriveSection = document.getElementById('manualDriveSection');

            if (state === 'stop') {
                document.getElementById('box-stop').classList.add('active-stop');
                runParamsSection.style.display = 'none';
                sensorSection.style.display = 'none';
                manualDriveSection.style.display = 'none';
            } else if (state === 'run') {
                document.getElementById('box-run').classList.add('active-run');
                runParamsSection.style.display = 'block';
                sensorSection.style.display = 'none';
                manualDriveSection.style.display = 'none';
            } else if (state === 'manual') {
                document.getElementById('box-manual').classList.add('active-manual');
                runParamsSection.style.display = 'none';
                sensorSection.style.display = 'block';
                manualDriveSection.style.display = 'block';
            }
        }

        function addSerialLine(text) {
            const monitor = document.getElementById('serialMonitor');
            monitor.textContent += '\n' + text;
            monitor.scrollTop = monitor.scrollHeight;
        }

        function sendCmd() {
            const input = document.getElementById('cmdInput');
            const cmd = input.value.trim();
            if (cmd === '') return;

            addSerialLine('> ' + cmd);
            if (websocket && websocket.readyState === WebSocket.OPEN) {
                websocket.send(cmd);
            } else {
                addSerialLine('[Fout: niet verbonden met robot]');
            }
            input.value = '';
        }

        function sendQuickCmd(cmd) {
            addSerialLine('> ' + cmd);
            if (websocket && websocket.readyState === WebSocket.OPEN) {
                websocket.send(cmd);
            } else {
                addSerialLine('[Fout: niet verbonden met robot]');
            }
        }

        function fillCmd(prefix) {
            const input = document.getElementById('cmdInput');
            input.value = prefix;
            input.focus();
        }
    </script>

</body>
</html>
)rawliteral";