from matplotlib.colors import LinearSegmentedColormap

def map_RB():
    # Create the custom colormap
    colors = ['#f6685e', '#ffffff', '#5393ff']  # Blue -> White -> Red # #00e676
    n_bins = 3  # Number of bins to use in the interpolation
    cmap_name = 'custom_colormap'
    return LinearSegmentedColormap.from_list(cmap_name, colors, N=3)

def map_RRBB():
    # Create the custom colormap
    colors = ['#ff383e', '#f6685e', '#ffffff', '#5393ff', '#3353ff']  # Blue -> White -> Red # #00e676
    n_bins = 5  # Number of bins to use in the interpolation
    cmap_name = 'custom_colormap'
    return LinearSegmentedColormap.from_list(cmap_name, colors, N=5)

def map_RRRBBB():
    # Create the custom colormap
    colors = ['#ff0000','#ff383e', '#f6685e','#f69999', '#ffffff', '#53ffff','#5393ff', '#3353ff', '#0000ff']  # Blue -> White -> Red # #00e676
    n_bins = 5  # Number of bins to use in the interpolation
    cmap_name = 'custom_colormap'
    return LinearSegmentedColormap.from_list(cmap_name, colors, N=7)