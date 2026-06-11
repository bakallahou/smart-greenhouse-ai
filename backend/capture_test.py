import cv2

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()

    cv2.imshow("Smart Greenhouse Camera", frame)

    key = cv2.waitKey(1)

    if key == ord("s"):
        cv2.imwrite("capture.jpg", frame)
        print("Image saved")
        break

cap.release()
cv2.destroyAllWindows()