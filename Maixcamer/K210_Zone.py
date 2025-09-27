import sensor, image, time, lcd

# 初始化屏幕和摄像头
lcd.init()  # 初始化LCD屏幕
sensor.reset()  # 重置摄像头
sensor.set_pixformat(sensor.RGB565)  # 使用彩色模式便于区分区域
sensor.set_framesize(sensor.QVGA)   # 设置分辨率为160x120（常用小尺寸）
sensor.skip_frames(time=2000)        # 等待摄像头稳定
clock = time.clock()

# 屏幕分区参数计算
screen_width = 320 # QQVGA宽度
screen_height = 240  # QQVGA高度
zone_width = screen_width // 3  # 每个区域的宽度（约53像素）

# 定义三个区域的坐标范围 (x起始, y起始, 宽度, 高度)
zones = {
    "left": (0, 0, zone_width, screen_height),
    "middle": (zone_width, 0, zone_width, screen_height),
    "right": (zone_width * 2, 0, zone_width, screen_height)
}

def draw_zones(img):
    """绘制绘制区域分隔线和区域标签"""
    # 绘制垂直分隔线（红色，线宽2像素）
    # 左区域与中区域的分隔线
    img.draw_line(zone_width, 0, zone_width, screen_height, color=(255, 0, 0), thickness=2)
    # 中区域与右区域的分隔线
    img.draw_line(zone_width * 2, 0, zone_width * 2, screen_height, color=(255, 0, 0), thickness=2)

    # 在每个区域左上角角标注区域名称（白色文字，黑色背景）
    # 左区域
    img.draw_string(10, 10, "LEFT", color=(255, 255, 255), bg_color=(0, 0, 0), scale=1)
    # 中区域
    img.draw_string(zone_width + 10, 10, "MID", color=(255, 255, 255), bg_color=(0, 0, 0), scale=1)
    # 右区域
    img.draw_string(zone_width * 2 + 10, 10, "RIGHT", color=(255, 255, 255), bg_color=(0, 0, 0), scale=1)

    return img

while True:
    clock.tick()
    img = sensor.snapshot()  # 获取摄像头图像

    # 绘制区域分隔线和标签
    img = draw_zones(img)

    # 在屏幕上显示处理后的图像
    lcd.display(img)

    # 打印帧率（调试用）
    # print("FPS:", clock.fps())
