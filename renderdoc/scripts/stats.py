import renderdoc as rd
from collections import deque
import struct

statistic_range = [[]]

def get_all_actions(controller: rd.ReplayController) -> dict:
    ret = dict()
    root_actions = controller.GetRootActions()
    temp_buffer = deque()

    for r in root_actions:
        temp_buffer.append(r)
    while len(temp_buffer) != 0:
        temp_top = temp_buffer.popleft()
        for d in temp_top.children:
            temp_buffer.append(d)
        ret[temp_top.eventId] = temp_top
    return ret

def get_all_time_duration(controller: rd.ReplayController, all_actions: dict) -> dict:
    counters = controller.EnumerateCounters()
    if rd.GPUCounter.EventGPUDuration not in counters:
        raise RuntimeError("GPU dose not support read time duration")

    samplePassedVal = controller.FetchCounters([rd.GPUCounter.EventGPUDuration])

    time_cost_result = dict()
    for result in samplePassedVal:
        temp_act = all_actions[result.eventId]
        if not (temp_act.flags & rd.ActionFlags.Drawcall):
            continue
        val = result.value.u64
        double_value = struct.unpack('d', struct.pack('Q', val))[0] * 1000
        time_cost_result[result.eventId] = double_value

    return time_cost_result

def get_triangle_count(all_actions: dict, time_cost_dict: dict, event_range: list):
    triangle_count = 0
    draw_count = 0
    time_cost = 0
    for action_id in all_actions:
        if len(event_range) == 2 and (action_id < event_range[0] or action_id > event_range[1]):
            continue
        rd_action = all_actions[action_id]
        if not (rd_action.flags & rd.ActionFlags.Drawcall):
            continue
        triangle_count += int((rd_action.numIndices / 3) * rd_action.numInstances)
        draw_count += 1
        time_cost += time_cost_dict[action_id]
    return triangle_count, draw_count, time_cost


def list_simple_info(all_actions: dict, time_cost: dict, event_range: list):
    for action_id in all_actions:
        if len(event_range) == 2 and (action_id < event_range[0] or action_id > event_range[1]):
            continue
        rd_action = all_actions[action_id]
        event_id = action_id
        if not (rd_action.flags & rd.ActionFlags.Drawcall):
            continue
        triangle_count = int((rd_action.numIndices / 3) * rd_action.numInstances)
        print("{},{},{}".format(event_id, triangle_count, time_cost[event_id]))

def run_quest(controller: rd.ReplayController):
    all_action = get_all_actions(controller)
    time_cost = get_all_time_duration(controller, all_action)

    print("Range,Triangle,DrawCall,TimeCost")
    for range in statistic_range:
        result = get_triangle_count(all_action, time_cost, range)
        print("{},{},{},{}".format(range, result[0], result[1], result[2]))

if __name__ == '__main__':
    # pyrenderdoc.SetEventID([], 51, 51)
    pyrenderdoc.Replay().BlockInvoke(run_quest)
    # pyrenderdoc.Replay().BlockInvoke(run_get_draw_index)

