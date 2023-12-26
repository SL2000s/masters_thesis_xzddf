import matplotlib.pyplot as plt
import numpy as np

def make_dir(labels, values):
    return dict(zip(labels, values))

if __name__ == '__main__':

    x = [0, 1, 10, 100, 1000]

    y_AP_128 =      [0, 349.68, 3476.35, 35364.2, 355842]
    y_GINX_128 =    [0, 292.52, 2898.16, 29324.7, 315418]
    y_LMKCDEY_128 = [0, 259.33, 2474.62, 28362, 280270]
    y_XZDDF_128 =   [0, 330.31, 3687.06, 33481, 331717]
    y_AP_192 =      [0, 1145.94, 13052.1, 115382, 1213410]
    y_GINX_192 =    [0, 949.26, 9417.36, 96319.4, 1005410]
    y_LMKCDEY_192 = [0, 853.6, 8171.51, 86081.9, 822024]
    y_XZDDF_192 =   [0, 1253.67, 12438.6, 114381, 1163360]

    method_key = 'method'
    security_key = 'security'
    tests_key = 'tests'
    labels =      [method_key, security_key, tests_key]

    AP_128 =      ['AP',       128,          y_AP_128]
    GINX_128 =    ['GINX',     128,          y_GINX_128]
    LMKCDEY_128 = ['LMKCDEY',  128,          y_LMKCDEY_128]
    XZDDF_128 =   ['XZDDF',    128,          y_XZDDF_128]
    AP_192 =      ['AP',       192,          y_AP_192]
    GINX_192 =    ['GINX',     192,          y_GINX_192]
    LMKCDEY_192 = ['LMKCDEY',  192,          y_LMKCDEY_192]
    XZDDF_192 =   ['XZDDF',    192,          y_XZDDF_192] 

    to_plot = {
        'AP_STD128': make_dir(labels, AP_128),
        'GINX_STD128': make_dir(labels, GINX_128),
        'LMKCDEY_STD128LMKCDEY': make_dir(labels, LMKCDEY_128),
        'XZDDF_STD128': make_dir(labels, XZDDF_128),
        'AP_STD192': make_dir(labels, AP_192),
        'GINX_STD192': make_dir(labels, GINX_192),
        'LMKCDEY_STD192': make_dir(labels, LMKCDEY_192),
        'XZDDF_STD192': make_dir(labels, XZDDF_192),
    }

    for over_name, over_dict in to_plot.items():
        y_times = over_dict[tests_key]
        plt.loglog(x, y_times, marker='o')
        for i, txt in enumerate(y_times):       # write y values at the data points
            plt.text(x[i], y_times[i], f'{y_times[i]:.2f}', ha='left', va='top')
        plt.title(f'Batch test for {over_dict[method_key]} ($\lambda={over_dict[security_key]}$ bits)')
        plt.xlabel('Number of (AND, OR, NOT) operations')
        plt.ylabel('Average time (ms)')
        plt.savefig(f'figs/{over_name}_batch.png')
        #plt.show()
        plt.close()