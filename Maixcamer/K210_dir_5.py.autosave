import sensor, image, lcd
from machine import UART
from fpioa_manager import fm

# 配置串口（P7=TX, P6=RX）
fm.register(7, fm.fpioa.UART1_TX, force=True)
fm.register(6, fm.fpioa.UART1_RX, force=True)
uart_A = UART(UART.UART1, 115200, 8, 0, 1, timeout=1000)

# 配置参数
black_threshold = ((0, 2))  # 黑色阈值
roi = [0, 100, 320, 16]      # 检测区域（中间偏下）
center_line = 160            # 屏幕中线位置
LEFT_THRESHOLD = -30         # 左偏阈值
RIGHT_THRESHOLD = 30         # 右偏阈值
LEFT_WIDTH_THRESHOLD = 140    # 左侧色块宽度阈值（超过则左转）
RIGHT_WIDTH_THRESHOLD = 140  # 右侧色块宽度阈值（超过则右转）

# 初始化
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)  # 320x240
sensor.skip_frames(time=3000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)

def send_direction(direction):
    ascii_char = chr(direction + 48)
    uart_A.write(ascii_char)
    # 打印发送的ASCII码信息
    print("发送ASCII字符: '" + ascii_char + "' (ASCII码值: " + str(ord(ascii_char)) + ")")
    return direction

while True:
    img = sensor.snapshot()
    blobs = img.find_blobs([black_threshold], roi=roi, area_threshold=200, merge=True)

    direction = 0  # 0=居中, 1=左偏, 2=右偏, 3=左转, 4=右转
    left_width = 0  # 左侧色块宽度
    right_width = 0 # 右侧色块宽度

    # 绘制中线作为参考
    img.draw_line(center_line, 100, center_line, 116, color=128)

    if blobs:
        b = blobs[0]
        img.draw_rectangle(b[0:4], color=255)
        img.draw_cross(b[5], b[6], color=255)

        # 计算色块左右部分的宽度
        # 色块左边界到中线的宽度（左侧宽度）
        if b[0] < center_line:
            left_width = min(center_line - b[0], b[2])
        # 色块右边界到中线的宽度（右侧宽度）
        if b[0] + b[2] > center_line:
            right_width = min((b[0] + b[2]) - center_line, b[2])

        # 显示左右宽度
        img.draw_string(10, 30, "Left Width: " + str(left_width), color=255)
        img.draw_string(160, 30, "Right Width: " + str(right_width), color=255)

        # 宽度判断优先级最高
        if left_width > LEFT_WIDTH_THRESHOLD:
            direction = 3  # 左侧过宽，发送左转指令
        elif right_width > RIGHT_WIDTH_THRESHOLD:
            direction = 4  # 右侧过宽，发送右转指令
        else:
            # 正常左右偏判断
            line_center = b[5]
            deviation = line_center - center_line
            if deviation < LEFT_THRESHOLD:
                direction = 1
            elif deviation > RIGHT_THRESHOLD:
                direction = 2
            else:
                direction = 0

    # 屏幕显示方向
    img.draw_string(10, 10, "Dir: " + str(direction), color=255)
    lcd.display(img)

    # 发送ASCII码值
    send_direction(direction)
    print("方向: {direction}, 左宽: {left_width}, 右宽: {right_width}")
