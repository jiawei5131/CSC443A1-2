import numpy as np
from matplotlib import pyplot
from csv import reader

with open("result/in_degree.csv", 'r') as in_result:
    in_degree_data = list(reader(in_result))
with open("result/out_degree.csv", 'r') as out_result:
    out_degree_data = list(reader(out_result))

max_degree = 100000

in_y = [ in_degree_data[i][1] for i in range(max_degree) ]
out_y = [ out_degree_data[i][1] for i in range(max_degree) ]

pyplot.title('In_degree And Out_degree Count \'log-log\' Plot')
pyplot.xscale('log')
pyplot.yscale('log')
pyplot.plot(range(max_degree), in_y, label = "in_degree")
pyplot.plot(range(max_degree), out_y, label = "out_degree")
pyplot.legend()
pyplot.show()
