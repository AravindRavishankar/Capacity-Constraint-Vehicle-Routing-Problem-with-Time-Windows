import sys
import math
import signal
from ortools.constraint_solver import pywrapcp
from ortools.constraint_solver import routing_enums_pb2


def distance(x1, y1, x2, y2):
    # Manhattan distance
    dist = (x1-x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)
    return float(dist)

# Distance callback
class CreateDistanceCallback(object):
    """Create callback to calculate distances and travel times between points."""
    def __init__(self, locations):
        """Initialize distance array."""
        num_locations = len(locations)
        self.matrix = {}

        for from_node in xrange(num_locations):
          self.matrix[from_node] = {}
          for to_node in xrange(num_locations):
            x1 = locations[from_node][0]
            y1 = locations[from_node][1]
            x2 = locations[to_node][0]
            y2 = locations[to_node][1]
            self.matrix[from_node][to_node] = float(distance(x1, y1, x2, y2))

    def Distance(self, from_node, to_node):
        return float(self.matrix[from_node][to_node])


# Demand callback
class CreateDemandCallback(object):
    """Create callback to get demands at location node."""
    def __init__(self, demands):
        self.matrix = demands

    def Demand(self, from_node, to_node):
        return self.matrix[from_node]

# Service time (proportional to demand) callback.
class CreateServiceTimeCallback(object):
    """Create callback to get time windows at each location."""
    def __init__(self, demands, time_per_demand_unit):
        self.matrix = demands
        self.time_per_demand_unit = float(time_per_demand_unit)

    def ServiceTime(self, from_node, to_node):
        return 0.0  #service time 0
        #return int(self.matrix[from_node] * self.time_per_demand_unit)

# Create the travel time callback (equals distance divided by speed).
class CreateTravelTimeCallback(object):
    """Create callback to get travel times between locations."""
    def __init__(self, dist_callback, speed):
        self.dist_callback = dist_callback
        self.speed = float(speed)

    def TravelTime(self, from_node, to_node):
        travel_time = float(self.dist_callback(from_node, to_node)) / float(self.speed)
        return travel_time

# Create total_time callback (equals service time plus travel time).
class CreateTotalTimeCallback(object):
    """Create callback to get total times between locations."""
    def __init__(self, service_time_callback, travel_time_callback):
        self.service_time_callback = service_time_callback
        self.travel_time_callback = travel_time_callback

    def TotalTime(self, from_node, to_node):
        service_time = float(self.service_time_callback(from_node, to_node))
        travel_time = float(self.travel_time_callback(from_node, to_node))
        return service_time + travel_time

def create_data_array(depot, error):
    locations, demands, start_times, end_times = [[[depot[0], depot[1]]], [0], [0], [1000000000]]
    for elem in depot[3]:
        locations.append([elem[1], elem[2]])
        demands.append(elem[5])
        start_times.append(max(0, elem[3] - error))
        end_times.append(elem[4] + error)
    data = [locations, demands, start_times, end_times]
    return data


def single_depot(depot, error):
    # Create the data.
    data = create_data_array(depot, error)
    locations = data[0]
    demands = data[1]
    start_times = data[2]
    end_times = data[3]
    num_locations = len(locations)
    depot = 0
    num_range = [100]
    ans_value = -1
    for num_vehicles in num_range:
        print "------ =======  For maximum number of vehicles used ", num_vehicles
        search_time_limit = 1
        pre_value = -1.0
        ret_value = -1.0

        # Create routing model.
        if num_locations > 0:

            # The number of nodes of the VRP is num_locations.
            # Nodes are indexed from 0 to num_locations - 1. By default the start of
            # a route is node 0.
            routing = pywrapcp.RoutingModel(num_locations, num_vehicles, depot)
            search_parameters = pywrapcp.RoutingModel.DefaultSearchParameters()

            # Callbacks to the distance function and travel time functions here.
            dist_between_locations = CreateDistanceCallback(locations)
            dist_callback = dist_between_locations.Distance

            routing.SetArcCostEvaluatorOfAllVehicles(dist_callback)
            demands_at_locations = CreateDemandCallback(demands)
            demands_callback = demands_at_locations.Demand

            # Adding capacity dimension constraints.
            VehicleCapacity = 1000000;
            NullCapacitySlack = 0;
            fix_start_cumul_to_zero = True
            capacity = "Capacity"

            routing.AddDimension(demands_callback, NullCapacitySlack, VehicleCapacity,
                                 fix_start_cumul_to_zero, capacity)
            # Add time dimension.
            time_per_demand_unit = 3
            horizon = 24 * 3600
            time = "Time"
            speed = 1.0

            service_times = CreateServiceTimeCallback(demands, time_per_demand_unit)
            service_time_callback = service_times.ServiceTime

            travel_times = CreateTravelTimeCallback(dist_callback, speed)
            travel_time_callback = travel_times.TravelTime

            total_times = CreateTotalTimeCallback(service_time_callback, travel_time_callback)
            total_time_callback = total_times.TotalTime

            routing.AddDimension(total_time_callback,  # total time function callback
                                 horizon,
                                 horizon,
                                 fix_start_cumul_to_zero,
                                 time)
            # Add time window constraints.
            time_dimension = routing.GetDimensionOrDie(time)
            for location in range(1, num_locations):
                start = start_times[location]
                end = end_times[location]
                time_dimension.CumulVar(location).SetRange(start, end)
            # Solve displays a solution if any.
            print "solving... "
            assignment = routing.SolveWithParameters(search_parameters)
            if assignment:
                size = len(locations)
                # Solution cost.
                #print "Total distance of all routes: " + str(math.sqrt(assignment.ObjectiveValue())) + "\n"
                ret_value = 0.0
                # Inspect solution.
                capacity_dimension = routing.GetDimensionOrDie(capacity);
                time_dimension = routing.GetDimensionOrDie(time);
                out_str = ""
                for vehicle_nbr in range(num_vehicles):
                    ctt = 0
                    index = routing.Start(vehicle_nbr)
                    plan_output = 'Vehicle '

                    while not routing.IsEnd(index):
                        node_index = routing.IndexToNode(index)
                        load_var = capacity_dimension.CumulVar(index)
                        time_var = time_dimension.CumulVar(index)
                        if pre_value == -1.0:
                            ret_value += math.sqrt(assignment.Min(time_var))
                            pre_value = assignment.Min(time_var)
                        else:
                            try: ret_value += math.sqrt(assignment.Min(time_var) - pre_value)
                            except: pass
                            pre_value = assignment.Min(time_var)
                        plan_output += \
                                " <{node_indexx}, {node_indexy}> Capacity({load}) Time({tmin}) -> ".format(
                                    node_indexx=locations[node_index][0],
                                    node_indexy=locations[node_index][1],
                                    load=assignment.Value(load_var),
                                    tmin=str(ret_value))
                        ctt += 1
                        index = assignment.Value(routing.NextVar(index))

                    node_index = routing.IndexToNode(index)
                    load_var = capacity_dimension.CumulVar(index)
                    time_var = time_dimension.CumulVar(index)
                    if pre_value == -1.0:
                        ret_value += math.sqrt(assignment.Min(time_var))
                        pre_value = assignment.Min(time_var)
                    else:
                        try: ret_value += math.sqrt(assignment.Min(time_var) - pre_value)
                        except: pass
                        pre_value = assignment.Min(time_var)
                    plan_output += \
                              " <{node_indexx}, {node_indexy}> Capacity({load}) Time({tmin}) -> ".format(
                                    node_indexx=locations[node_index][0],
                                    node_indexy=locations[node_index][1],
                                    load=assignment.Value(load_var),
                                    tmin=str(ret_value))
                    if ctt > 1:
                        out_str += plan_output
                        out_str += "\n"
                print out_str
            else: print 'No solution found.'
        else: print 'Specify an instance greater than 0.'
        ans_value = max(ans_value, ret_value)
    return ans_value



input_file = open(sys.argv[1], 'r')
depots = []
n_depots = 0
customers = []
n_customers = 0

def depot_unassign():
    global depots
    for i in range(0, len(depots)):
        depots[i][3] = []
        depots[i][4] = 0

def assign_to_depot():
    global depots, customers
    for i in range(len(customers)-1, -1, -1):
        cust_x = customers[i][1]
        cust_y = customers[i][2]
        demand = customers[i][5]
        nearest_depots = []
        for elem in range(0, len(depots)):
            x = depots[elem][0]
            y = depots[elem][1]
            dist = distance(x, y, cust_x, cust_y)
            nearest_depots.append([dist, elem])
        nearest_depots.sort(key = lambda nearest_depots: nearest_depots[0])
        assigned = False
        for elem in nearest_depots:
            cur = depots[elem[1]][4]
            cap = depots[elem[1]][2]
            if cur + demand <= cap:
                depots[elem[1]][3].append(customers[i])
                assigned = True
                break
        if assigned == False: continue



cond = 0
ct = 0
for line in input_file.readlines():
    if cond == 0: 
        n_depots = int(line.strip())
        cond = 1
    elif cond == 1:
        x, y, cap = line.split()
        depots.append([int(x), int(y), int(cap), [], 0])  #depots stored as [x, y, capacity, customers assigned, start time]
        ct += 1
        if ct == n_depots: cond = 2
    elif cond == 2:
        n_customers = int(line.strip())
        cond = 3
        ct = 0
    elif cond == 3:
        x, y, e, l, d, m = line.split()
        customers.append([int(m), int(x), int(y), int(e), int(l), int(d)])    #customers stored as [multiplier, x, y, ear, lat, demand]
        if ct == n_customers: cond = 4
    else: pass

max_error = 100
for error in range(0, max_error+1, 10):
    print "\n\n--------------------------------------------------"
    print "MAX ERROR LIMIT: " + str(error)
    print "--------------------------------------------------"
    #sorting based on priority'''
    depot_unassign()
    customers.sort(key = lambda customers: customers[0])
    assign_to_depot()
    dist = 0
    depot_ct = 0
    for elem in depots:
        print "<<Depot " + str(depot_ct) + ">>"
        depot_ct += 1
        result = single_depot(elem, error)
        if result > 0.0: dist += result
    print "=====>>Sorting Based on Priority:"
    print "********************"
    print "Total distance of all routes: " + str(dist)
    print "********************\n"
    #sorting based on earlist time and latest time
    '''
    depot_unassign()
    customers.sort(key = lambda customers: (customers[3], customers[4]))
    assign_to_depot()
    dist = 0
    for elem in depots:
        dist += single_depot(elem, error)
    print "=====>>Sorting Based on earliest time and latest time:"
    print "********************"
    print "Total distance of all routes: " + str(dist)
    print "********************\n"

    #sorting based on latest time
    depot_unassign()
    customers.sort(key = lambda customers: customers[4])
    assign_to_depot()
    dist = 0
    for elem in depots:
        dist += single_depot(elem, error)
    print "=====>>Sorting Based on latest time:"
    print "********************"
    print "Total distance of all routes: " + str(dist)
    print "********************\n"
    '''


