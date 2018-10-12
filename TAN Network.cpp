#include <iostream>
#include <math.h> // used for PI, cos, and sqrt
#include <sstream> // used for parcing
#include <map>
#include <queue> // used for a priority_queue
#include <vector>
//#include <iomanip> // for setprecision()

/**
 * This program takes several inputs about a number of
 * transportation stops and uses Dijkstra's algorithm to
 * determine the shortest path from one specified stop to
 * another specified stop.
 **/

struct Route
{
    std::string stop;
    double distance;
    
    inline Route( const std::string &input_stop, const double &input_distance )
    {
        stop = input_stop;
        distance = input_distance;
    }
};
    
class TAN_Stop
{
    public:
        std::string ID,
                    name,
                    description, // not used
                    identifier, // not used
                    url, // not used
                    type_,
                    mother_station, // not used
                    shortest_route;
        double latitude, // in degrees
                longitude, // in degrees
                shortest_distance;
        std::vector<Route> routes;
        
        inline TAN_Stop( std::stringstream input_stream )
        {
            std::string latitude_string, longitude_string;
                
            for( std::string* item: { &ID, &name, &description, &latitude_string, 
                &longitude_string, &identifier, &url, &type_, &mother_station } )
            {
                getline( input_stream, *item, ',' );
            }
                
            name.erase( 0, 1 ); // remove the first quote
            name.pop_back(); // remove the last quote
                
            latitude = stod( latitude_string );
            longitude = stod( longitude_string );
                
            shortest_route = "";
        }
        
        inline void add_stop( TAN_Stop &input_stop )
        { // This function adds the input_stop to this stop's routes
            double x_distance = ( input_stop.longitude - longitude ) * cos( M_PI * ( latitude + input_stop.latitude ) / 360 );
            double y_distance = input_stop.latitude - latitude;
            double distance = 6371 * sqrt( x_distance * x_distance + y_distance * y_distance );
        
            routes.emplace_back( Route( input_stop.ID, distance ) );
        }
            
        const inline void debug_print()
        { // This function prints every element the object has
            for( const std::string* item: { &ID, &name, &description } )
            {
                std::cerr << *item << std::endl;
            }
            
            std::cerr << latitude << std::endl << longitude << std::endl;
                
            for( const std::string* item: { &identifier, &url, &type_, &mother_station, &shortest_route } )
            {
                std::cerr << *item << std::endl;
            }
                
            std::cerr << shortest_distance << std::endl;
            
            for( const Route &route_iterator: routes )
            {
                std::cerr << "Route: " << route_iterator.stop << std::endl
                    << "Route: " << route_iterator.distance << std::endl;
            }
                
            std::cerr << std::endl;
        }
};

std::map<std::string, TAN_Stop> stops; // made global for the preceding functions

struct compare_distance
{ // Only used with the priority queue to organize inserted stops.
    bool operator()( const std::string &input_string_A, const std::string &input_string_B )
    { // This function returns true if the first argument has a greater distance than the second argument.
        return stops.at( input_string_A ).shortest_distance > stops.at( input_string_B ).shortest_distance;
    }
};
    
const inline void debug_all()
{ // This function prints out every aspect of every stop avalable
    for( std::pair<std::string, TAN_Stop> key: stops )
    {
        std::cerr << key.first << " <key>" << std::endl;
        key.second.debug_print();
    }
}

const inline void debug_queue( std::priority_queue<std::string, std::vector<std::string>, compare_distance> input_queue )
{ // This function makes a copy of the priority_queue passed to it and displays each element within it
    if( input_queue.empty() )
    {
        std::cerr << "Queue is empty" << std::endl;
    }
    else
    {
        while( !input_queue.empty() )
        {
            std::cerr << "Queue debug: " << input_queue.top() << "; "
                << stops.at( input_queue.top() ).shortest_distance << std::endl;
            input_queue.pop(); // the reason a copy is needed. traversing the queue requires deconstructing it
        }
    }
}
            
int main()
{
    //cerr << setprecision(10); // show 10 digits, to ensure the data is parsed correctly
    
    std::string start_point, end_point;
    int stop_count, route_count;
    
    std::cin >> start_point; std::cin.ignore();
    std::cin >> end_point; std::cin.ignore();
    
    { // block only to end scope for input_string
        std::string input_string, input_string_argument;
        
        std::cin >> stop_count; std::cin.ignore();
        for( int count = stop_count; count; --count )
        { // for every stop input
            getline( std::cin, input_string );
            stops.emplace( input_string.substr( 0, input_string.find( ',' ) ),
                            TAN_Stop( std::stringstream( input_string ) ) );
        }
    
        std::cin >> route_count; std::cin.ignore();
        for( int count = route_count; count; --count )
        { // for every route input
            getline( std::cin, input_string, ' ' );
            getline( std::cin, input_string_argument );
            stops.at( input_string ).add_stop( stops.at( input_string_argument ) );
        }
    }
    
    std::priority_queue<std::string, std::vector<std::string>, compare_distance> search_stops;
    
    for( Route& route_iterator: stops.at( start_point ).routes ) // initializing the distance table and priority queue
    {
        stops.at( route_iterator.stop ).shortest_route = start_point;
        stops.at( route_iterator.stop ).shortest_distance = route_iterator.distance;
        search_stops.push( route_iterator.stop );
    }
    
    // Dijkstra's
    while( !search_stops.empty() && search_stops.top() != end_point )
    {
        for( Route &route_iterator: stops.at( search_stops.top() ).routes ) // for each route from this stop
        {
            if( stops.at( route_iterator.stop ).shortest_route.empty() ||
                stops.at( route_iterator.stop ).shortest_distance > 
                route_iterator.distance + stops.at( search_stops.top() ).shortest_distance ) 
            {
                stops.at( route_iterator.stop ).shortest_distance = 
                    route_iterator.distance + stops.at( search_stops.top() ).shortest_distance; // update with shorter path

                if( stops.at( route_iterator.stop ).shortest_route.empty() )
                {
                    search_stops.push( route_iterator.stop ); // add the next stop
                }
                
                stops.at( route_iterator.stop ).shortest_route = search_stops.top(); // the stop that reaches the new one
            }
        }
        
        search_stops.pop(); // remove the evaluated stop
    }
    
    if( stops.at( end_point ).shortest_route.empty() )
    { // the end_point is not reachable from the start_point
        std::cout << "IMPOSSIBLE" << std::endl;
    }
    else
    {
        std::vector<std::string> route_sequence ( 1, end_point );

        while( route_sequence.back() != start_point )
        { // work though the connections to reach the start_point
            route_sequence.emplace_back( stops.at( route_sequence.back() ).shortest_route ); // add to the front
        }

        for( std::vector<std::string>::reverse_iterator route_iterator = route_sequence.rbegin();
                                    route_iterator != route_sequence.rend(); ++route_iterator )
        {
            std::cout << stops.at( *route_iterator ).name << std::endl;
        }
    }
}