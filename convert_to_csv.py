import scipy.io
import pandas as pd
import matplotlib.image as img


def convert_mat_to_csv(file_path, variable):
    mat = scipy.io.loadmat(file_path)
    mat = mat.get(variable)
    data = pd.DataFrame(mat)
    data.to_csv(file_path+".csv", header=False, index=False)


def convert_img_to_csv(file_path):
    image = img.imread(file_path)
    # if image is colored (RGB)
    if image.shape[2] == 3:
        # reshape it from 3D matrix to 2D matrix
        image_reshape = image.reshape(image.shape[0], -1)
    # if image is grayscale
    else:
        # remain as it is
        image_reshape = image

    # converting it to dataframe.
    mat_df = pd.DataFrame(image_reshape)
    mat_df = mat_df/255

    # exporting dataframe to CSV file.
    mat_df.to_csv(file_path + '.csv',
                  header=False,
                  index=False)

