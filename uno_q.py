# SPDX-FileCopyrightText: Copyright (C) Arduino s.r.l. and/or its affiliated companies
# SPDX-License-Identifier: MPL-2.0

from datetime import datetime, UTC
from arduino.app_utils import App, Bridge
from arduino.app_bricks.web_ui import WebUI
from arduino.app_bricks.video_objectdetection import VideoObjectDetection

ui = WebUI()
detection_stream = VideoObjectDetection(confidence=0.5, debounce_sec=0.0)

current_led_state = -1  # Forces initial state sync

ui.on_message("override_th", lambda sid, threshold: detection_stream.override_threshold(threshold))

def process_detections(detections: dict):
    global current_led_state
    elephant_detected = False

    for label, values in detections.items():
        for value in values:
            confidence = value.get("confidence", 0.0)
            
            # Send live results to UI
            entry = {
                "content": label,
                "confidence": confidence,
                "timestamp": datetime.now(UTC).isoformat()
            }
            ui.send_message("detection", message=entry)

            # Check for elephant with >85% confidence
            if "elephant" in label.lower() and confidence > 0.85:
                elephant_detected = True

    # 1 = ON when detected, 0 = OFF when not detected
    target_state = 1 if elephant_detected else 0

    # Only update hardware when detection state changes
    if target_state != current_led_state:
        try:
            Bridge.call("set_led", target_state)
            current_led_state = target_state
            print(f"[EVENT] Elephant present: {elephant_detected} -> LED {'ON' if target_state == 1 else 'OFF'}", flush=True)
        except Exception as err:
            print(f"[BRIDGE ERROR] {err}", flush=True)

detection_stream.on_detect_all(process_detections)

App.run()