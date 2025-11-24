import re
import json

class SpeedScopeTree:
    def __init__(self):
        self._root = {'root': { 'size': 0, 'children': {} }}
    
    def add_path(self, path, value):
        cur = self._root['root']
        for node in path:
            if node not in cur['children']:
                cur['children'][node] = { 'size': 0, 'children': {} }
            cur = cur['children'][node]
        cur['size'] += value

    def dump(self, filename):
        ''' 输出 SpeedScope 格式文件 '''
        data = { 'head': self.__dump_child('root', self._root['root']) }
        with open(filename, 'w', encoding='utf-8') as json_file:
            json.dump(data, json_file, ensure_ascii=False, indent=2)
    
    def __create_speedscope_node(self, name, node, children=[]):
        # 不支持负数
        if node['size'] < 0:
            return { 'callFrame': { 'functionName': name}, 'selfSize': 0, 'children': children }
        return { 'callFrame': { 'functionName': name}, 'selfSize': node['size'], 'children': children }

    def __dump_child(self, name, node):
        if len(node['children']) == 0:
            return self.__create_speedscope_node(name, node)
        
        children = []
        for cname, cnode in node['children'].items():
            children.append(self.__dump_child(cname, cnode))
        
        return self.__create_speedscope_node(name, node, children)


def tobyte(size, unit):
    if 'M' == unit:
        return size * 1024 * 1024
    if 'K' == unit:
        return size * 1024
    if 'bytes' == unit:
        return size

def topath(stack):
    path = []
    for item in stack:
        path.append(item[1])

# 满足条件的才会记录
def check(path, value, tag=''):
    if tag == '':
        return True
    # if 'malloc' not in path[-1]:
    #     return False
    for line in path:
        if tag in line:        # il2cpp虚拟机 先忽略
            return True
    return False

def main(file, max_depth, tag=''):
    sst = SpeedScopeTree()
    with open(file, 'r') as f:
        data = f.read()
        lines = re.findall(r'^\W{4}([ +!:|]*)(\d+) \(([\d.]+)([MKbytes]+)\) (.*)$', data, re.M)
    
    stack = []
    for m in lines:
        depth = len(m[0]) / 2
        if depth > max_depth:
            continue

        while depth < len(stack):
            path = []
            value = stack[-1][0]
            for item in stack:
                item[0] -= value
                path.append(item[1])

            # 只有满足条件才会记录
            if check(path, value, tag):
                sst.add_path(path, value)
            stack.pop()

        bsize = tobyte(float(m[2]), m[3])
        name = m[4]

        stack.append([bsize, name])
    sst.dump(f'{file}_{tag}.json')



if __name__ == '__main__':
    import sys
    print(f'input: {sys.argv[1]}')
    print(f'max depth: {sys.argv[2]}')
    main(sys.argv[1], int(sys.argv[2]), sys.argv[3])
