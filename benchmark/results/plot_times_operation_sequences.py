from math import ceil
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

    subplot_cols = 2
    subplot_rows = ceil(len(to_plot)/subplot_cols)
    fig, axs = plt.subplots(subplot_rows, subplot_cols)
    fig.suptitle('Batch tests', fontsize=16)
    for i, (over_name, over_dict) in enumerate(to_plot.items()):
        y_times = over_dict[tests_key]
        
        r = i // subplot_cols
        c = i % subplot_cols
        ax = axs[r, c]
        ax.loglog(x, y_times, marker='o')
        ax.set_title(f'{over_dict[method_key]} ($\lambda={over_dict[security_key]}$ bits)')
        ax.set(xlabel='#(AND, OR, NOT)', ylabel='Average time (ms)')
    fig.set_figheight(1.9*fig.get_figheight())
    fig.tight_layout()

    # write y values at the data points
    for i, (over_name, over_dict) in enumerate(to_plot.items()):
        y_times = over_dict[tests_key]
        r = i // subplot_cols
        c = i % subplot_cols
        for j, (x_val, y_val) in enumerate(zip(x, y_times)):       # write y values at the data points
            if j == len(x)-1:
                ha = 'right'
                va = 'top'
                x_val *= 0.7
                y_val *= 1.25
            else:
                ha = 'left'
                va = 'top'
                x_val *= 1.35
                y_val *= 1.35
            axs[r, c].text(x_val, y_val, f'{y_val:.2f}', ha=ha, va=va)

    # fig.show()
    plt.savefig('figs/batch.png')