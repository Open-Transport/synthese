import pstats

def generate_stats(stat_name):
    stream = open('{0}.prof_result'.format(stat_name), 'w')
    p = pstats.Stats('{0}.prof'.format(stat_name), stream=stream)
    #kind = 'cumulative'
    kind = 'time'
    p.sort_stats(kind).print_stats()

for stat_name in ('httpd', 'dispatcher'):
    generate_stats(stat_name)
