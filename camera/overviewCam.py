import cv2
import os

windowName = "cam1"

cv2.namedWindow(windowName)
vc = cv2.VideoCapture("rtsp://tapoadmin:"+os.getenv('TAPO_PASS')+"@10.2.172.155/stream1")


def rescale_frame(frame, percent=75):
    width = int(frame.shape[1] * percent/ 100)
    height = int(frame.shape[0] * percent/ 100)
    dim = (width, height)
    return cv2.resize(frame, dim, interpolation =cv2.INTER_AREA)


if vc.isOpened(): # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

while rval:
    frame = rescale_frame(frame,75)
    cv2.imshow(windowName, frame)
    rval, frame = vc.read()
    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break

vc.release()
cv2.destroyWindow(windowName)
