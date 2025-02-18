import os
from PIL import Image

def parse_paths(root_dir, ext_name = ".png"):
    file_paths = []
    for root, dirs, files in os.walk(root_dir):
        for file_name in files:
            if not file_name.endswith(ext_name):
                continue
            file_paths.append(os.path.join(root, file_name))
    #
    return file_paths

def trim_images():
    this_dir = os.path.dirname(__file__)
    png_paths = parse_paths(this_dir)
    for file_path in png_paths:
        img = Image.open(file_path)
        img_width, img_height = img.size
        img_box = img.getbbox()
        img_box = [img_box[0] - 1, img_box[1] - 1, img_box[2] + 1, img_box[3] + 1]
        img = img.crop(img_box)
        img.save(file_path)
#
trim_images()
