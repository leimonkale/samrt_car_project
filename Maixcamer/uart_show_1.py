import time
from machine import UART
from fpioa_manager import fm
import lcd

# 初始化LCD屏幕
lcd.init()
lcd.clear(lcd.WHITE)  # 白色背景

# 配置UART引脚（P6=RX, P7=TX）
fm.register(6, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)

# 初始化UART，波特率115200
uart = UART(UART.UART1, 115200, 8, 0, 0, timeout=100)

# 显示相关参数
LINE_HEIGHT = 16  # 每行高度
MAX_LINES = 7     # 屏幕最大行数（120/16≈7）
current_line = 0  # 当前行位置

def clear_screen():
    """清屏并重置行位置"""
    global current_line
    lcd.clear(lcd.WHITE)
    current_line = 0
    # 显示标题
    lcd.draw_string(10, 0, "UART Data:", lcd.BLACK, lcd.WHITE)
    current_line += 1

# 初始清屏并显示标题
clear_screen()

try:
    while True:
        # 读取UART数据（最多128字节）
        data = uart.read(128)
        if data:
            # 将字节数据转换为字符串
            try:
                text = data.decode('utf-8').strip()
                print("Received:", text)  # 同时输出到终端

                # 检查是否需要清屏（超过最大行数）
                if current_line >= MAX_LINES:
                    clear_screen()

                # 计算显示位置
                y_pos = current_line * LINE_HEIGHT
                # 在当前行显示数据
                lcd.draw_string(10, y_pos, text, lcd.BLACK, lcd.WHITE)
                current_line += 1

            except UnicodeDecodeError:
                # 处理无法解码的二进制数据
                error_msg = "Invalid UTF-8 data"
                lcd.draw_string(10, current_line * LINE_HEIGHT, error_msg, lcd.RED, lcd.WHITE)
                current_line += 1
                print(error_msg)

        time.sleep_ms(100)  # 短暂延时，降低CPU占用

except KeyboardInterrupt:
    # 程序中断处理
    lcd.clear(lcd.WHITE)
    lcd.draw_string(30, 50, "Stopped", lcd.RED, lcd.WHITE)
    print("Program stopped")

finally:
    # 释放资源
    uart.deinit()

    fm.unregister(7, force=True)
