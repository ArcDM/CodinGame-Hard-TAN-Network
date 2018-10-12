import math

"""
    This program takes several inputs about a number of
    transportation stops and uses Dijkstra's algorithm to
    determine the shortest path from one specified stop to
    another specified stop.
    
    This is the first python program that I've made that uses
    classes. I would have usually use a dictionary here.
    Using the classes makes the code neater, more readable and
    eliminates the stucture needed for the unutilized dictionary.
"""

class Route: # more of a stuct than anything
    def __init__( self, input_stop = None, input_distance = None ):
        self.stop = input_stop
        self.distance = input_distance
        
class TAN_Stop: # barely more than a struct
    def __init__( self, input_ID, input_name, input_description, input_latitude, input_longitude,
                    input_identifier, input_url, input_type_, input_mother_station ):
        self.ID = input_ID
        self.name = input_name.strip('"')
        self.description = input_description # not used
        self.latitude = float( input_latitude ) # in degrees
        self.longitude = float( input_longitude ) # in degrees
        self.identifier = input_identifier # not used
        self.url = input_url # not used
        self.type_ = input_type_
        self.mother_station = input_mother_station # not used
        self.routes = []
        self.shortest_route = None
        self.shortest_distance = None
        
    def add_stop( self, input_stop ):
        x_distance = ( input_stop.longitude - self.longitude ) * math.cos( math.pi * ( self.latitude + input_stop.latitude ) / 360 )
        y_distance = input_stop.latitude - self.latitude
        distance = 6371 * math.sqrt( x_distance ** 2 + y_distance ** 2 )
        
        self.routes.append( Route( input_stop.ID, distance ) )

start_point = input()
end_point = input()
stops = {}

for index in range( int( input() ) ) : # for every stop input
    stop_input = input().split(',')
    stops.update( { stop_input[ 0 ]: TAN_Stop( *stop_input ) } )
    
for index in range( int( input() ) ) : # for every route input
    stop_A, stop_B = input().split(' ')
    stops[ stop_A ].add_stop( stops[ stop_B ] )

for route in stops[ start_point ].routes : # initializing the distance table
    stops[ route.stop ].shortest_route = start_point
    stops[ route.stop ].shortest_distance = route.distance

search_stops = [ route.stop for route in sorted( stops[ start_point ].routes, key=lambda k: k.distance ) ]

# Dijkstra's
while search_stops and search_stops[ 0 ] != end_point: # while not empty
    evaluated_stop = search_stops.pop( 0 ) # removes from list and sets value

    for route in stops[ evaluated_stop ].routes : # for each route from this stop
        if( stops[ route.stop ].shortest_route is None or
        stops[ route.stop ].shortest_distance > route.distance + stops[ evaluated_stop ].shortest_distance ) :
            
            stops[ route.stop ].shortest_distance = route.distance + stops[ evaluated_stop ].shortest_distance # update with shorter path

            if stops[ route.stop ].shortest_route is None:
                low_index = 0
                high_index = len( search_stops )
            
                while low_index < high_index: # binary search for inordered spot for the next stop
                    middle_index = ( low_index + high_index ) // 2
                    
                    if stops[ search_stops[ middle_index ] ].shortest_distance < stops[ route.stop ].shortest_distance:
                        low_index = middle_index + 1
                    else:
                        high_index = middle_index
        
                search_stops.insert( low_index, route.stop ) # add the next stop
            
            stops[ route.stop ].shortest_route = evaluated_stop # the stop that reaches the new one

if stops[ end_point ].shortest_route :
    route_sequence = [ end_point ]

    while route_sequence[ 0 ] != start_point : # work though the connections to reach the start_point
        route_sequence.insert( 0, stops[ route_sequence[ 0 ] ].shortest_route ) # add to the front

    for result_index in route_sequence :
        print( stops[ result_index ].name )
else : # the end_point is not reachable from the start_point
    print( "IMPOSSIBLE" )