import os
from PIL import Image

padding_top = 1
padding_bottom = 1
padding_left = 1
padding_right = 1

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
        img_box = [img_box[0] - padding_top, img_box[1] - padding_left, img_box[2] + padding_bottom, img_box[3] + padding_right]
        x = img_box[2] - img_box[0]
        y = img_box[3] - img_box[1]
        suffix = "_t{0}_b{1}_l{2}_r{3}_{4}x{5}".format(padding_top, padding_bottom, padding_left, padding_right, x, y)
        img = img.crop(img_box)
        file_path = file_path[0:-4] + suffix + '.png'
        print(file_path + '\n')
        img.save(file_path)
#

trim_images()
