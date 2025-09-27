from machine import UART
from fpioa_manager import fm
import time

# 直接使用引脚编号映射（6对应P6，7对应P7）
# 将引脚6映射为UART1接收端(RX)，引脚7映射为UART1发送端(TX)
fm.register(6, fm.fpioa.UART1_RX, force=True)
fm.register(7, fm.fpioa.UART1_TX, force=True)

# 初始化UART1，波特率115200
uart = UART(UART.UART1, 115200, 8, 0, 0, timeout=1000)

try:
    while True:
        # 发送"010"并换行
        uart.write("010\r\n")
        print("已发送: 010")  # 同步打印到IDE控制台
        time.sleep_ms(500)  # 每500毫秒发送一次

except KeyboardInterrupt:
    # 手动中断时释放资源
    uart.deinit()
    fm.unregister(6, force=True)
    fm.unregister(7, force=True)
    print("程序已停止")

