import sensor, image, time, lcd

# 初始化LCD屏幕
lcd.init()  # 启动屏幕
lcd.clear(lcd.WHITE)  # 清屏为白色背景

# 初始化摄像头
sensor.reset()  # 重置摄像头
sensor.set_pixformat(sensor.RGB565)  # 设置像素格式为彩色
sensor.set_framesize(sensor.QVGA)  # 设置分辨率为320x240（适合大多数屏幕）
sensor.skip_frames(time=2000)  # 等待2秒让摄像头稳定
clock = time.clock()  # 创建时钟对象用于计算帧率

# 主循环：不断采集并显示图像
while True:
    clock.tick()  # 更新帧率计数
    img = sensor.snapshot()  # 拍摄一张图像
    lcd.display(img)  # 在屏幕上显示图像
    print("FPS:", clock.fps())  # 打印帧率（调试用）
