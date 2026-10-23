import cv2
from ultralytics import YOLO
import pyrealsense2 as rs
import numpy as np

model = YOLO('yolo26n.pt')
pipe = rs.pipeline()
cfg = rs.config()

width, height = 848, 480
point = (width // 2, height // 2)

closest_person_id = None

cfg.enable_stream(rs.stream.color, width, height, rs.format.bgr8, 30)
cfg.enable_stream(rs.stream.depth, width, height, rs.format.z16, 30)

align_to = rs.stream.color
align = rs.align(align_to)

pipe.start(cfg)

while True:
    frames = pipe.wait_for_frames()

    aligned_frames = align.process(frames)
    color_frame = aligned_frames.get_color_frame()
    depth_frame = aligned_frames.get_depth_frame()

    if not color_frame or not depth_frame:
        continue

    color_image = np.asanyarray(color_frame.get_data()) 
    
    closest_person_box = None
    closest_person_center = None
    target_person_distance = 0.0
    closest_person_conf = None
    min_distance = float('inf')

    results = model.track(color_image, classes=[0], persist=True, verbose=False, tracker="bytetrack.yaml") 

    if results[0].boxes is not None and results[0].boxes.id is not None:
        boxes=results[0].boxes.xyxy.cpu().numpy()
        track_ids= results[0].boxes.id.cpu().numpy()
        confs = results[0].boxes.conf.cpu().numpy()
        
        for box, track_id, conf in zip(boxes, track_ids, confs):
            x1, y1, x2, y2 = map(int, box)
            track_id = int0055(track_id)
            center_pt = ((x1 + x2) // 2, (y1 + y2) // 2)
            conf = float(conf)

            cx = max(0, min(center_pt[0], width - 1))
            cy = max(0, min(center_pt[1], height - 1))

            person_distance = depth_frame.get_distance(cx, cy)

            if 0 < person_distance < min_distance:
                min_distance = person_distance
                closest_person_id = track_id
                closest_person_box = (x1, y1, x2, y2)
                closest_person_center = (cx, cy)
                target_person_distance = person_distance
                closest_person_conf = conf

    camera_distance = depth_frame.get_distance(center_pt[0], center_pt[1])

    if closest_person_box is not None and closest_person_center is not None:
        x1, y1, x2, y2 = closest_person_box

        delta_x = closest_person_center[0] - point[0]
        delta_y = closest_person_center[1] - point[1]

        cv2.rectangle(color_image, (x1, y1), (x2, y2), (0, 255, 0), 2)
        cv2.line(color_image, point, closest_person_center, (0, 0, 255), 10)
        cv2.circle(color_image, closest_person_center, 20, (255, 0, 0), -1)
        cv2.circle(color_image, point, 20, (0, 255, 0), -1)

        cv2.putText(color_image, "id: {}".format(closest_person_id), (x1, y1 - 40), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
        cv2.putText(color_image, "conf: {:.2f}%".format(closest_person_conf*100), (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
        cv2.putText(color_image, "delta_x: {}px".format(delta_x), (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
        cv2.putText(color_image, "delta_y: {}px".format(delta_y), (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
        cv2.putText(color_image, "{}m".format(round(target_person_distance, 2)), (closest_person_center[0] + 10, closest_person_center[1] + 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)

        if round(target_person_distance, 2) > 0.5 and round(camera_distance, 2) > 0.5:
            if delta_x > 0 and delta_x > 50:
                cv2.putText(color_image, "Turn Right", (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
            elif delta_x < 0 and delta_x < -50:
                cv2.putText(color_image, "Turn Left", (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
            else:
                cv2.putText(color_image, "Go Straight", (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
        else:
            cv2.putText(color_image, "Stop", (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

    cv2.imshow("YOLO Live Depthcam", color_image)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
pipe.stop()
cv2.destroyAllWindows()
