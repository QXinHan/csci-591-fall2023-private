import networkx as nx
import matplotlib.pyplot as plt

'''
I just note the function developed by the writer not the API,
so each node represents a function or a code module developed by the developer.
Each directed edge represents the relationship between two node,
such as, function A invokes function B and how it works. 
'''

# draw a directed graph to show the function invotion
G = nx.DiGraph()

# add nodes
# nodes's attributes are return value type, function name, parameters
G.add_node(0,name='main',paramtyp='void',rettype='int',fun=True)
G.add_node(1,name='WriteData',paramtyp='string',rettype='void',fun=True)
G.add_node(2,name='SpecialKey',paramtyp='int',rettype='bool',fun=True)
G.add_node(3,name='logger',paramtyp='void',rettype='void',fun=True)
G.add_node(4,name='screens',paramtyp='void',rettype='void',fun=True)
G.add_node(5,name='CopyFile',paramtyp='LPCSTR,LPCSTR,BOOL',rettype='BOOL',fun=True)
G.add_node(6,name='strcpy',paramtyp='char*,const char*',rettype='BOOL',fun=True)
G.add_node(7,name='wstring',paramtyp='_iter,_iter',rettype='wstring',fun=True)
G.add_node(8,name='util.HideConsole',paramtyp='void',rettype='void',fun=False)
G.add_node(9,name='util.Autoload',paramtyp='string,string',rettype='void',fun=False)
G.add_node(10,name='util.FindPath',paramtyp='void',rettype='string',fun=False)
G.add_node(11,name='util.Path',paramtyp='void',rettype='LPCWSTR',fun=False)
G.add_node(12,name='util.FindDir',paramtyp='void',rettype='string',fun=False)
G.add_node(13,name='util.Screenshot',paramtyp='int,int,int,int,char*,HWND',rettype='bool',fun=False)
G.add_node(14,name='(ofstream LogFile)LogFile.open',paramtyp='const char*',rettype='ios_base::openmode',fun=False)
G.add_node(15,name='(thread logger(logger))logger.joinable',paramtyp='void',rettype='void',fun=False)
G.add_node(16,name='(thread logger(logger))logger.join',paramtyp='void',rettype='void',fun=False)
G.add_node(17,name='(thread screens(screens))screens.joinable',paramtyp='void',rettype='void',fun=False)
G.add_node(18,name='(thread screens(screens))screens.join',paramtyp='void',rettype='void',fun=False)
G.add_node(19,name='_access',paramtyp='const char*,int',rettype='int',fun=True)
G.add_node(19,name='_mkdir',paramtyp='const char*',rettype='int',fun=True)
G.add_node(20,name='AllocConsole',paramtyp='void',rettype='void',fun=True)
G.add_node(21,name='FindWindowA',paramtyp='LPCSTR,LPCSTR',rettype='HANDLE',fun=True)
G.add_node(22,name='ShowWindow',paramtyp='HWND,int',rettype='BOOL',fun=True)
G.add_node(23,name='system',paramtyp='const char*',rettype='bool',fun=True)
G.add_node(24,name='GetFilePointer',paramtyp='HANDLE',rettype='int',fun=False)
G.add_node(25,name='setFilePointer',paramtyp='HANDLE,int,int,int',rettype='int',fun=True)
G.add_node(26,name='SaveBMPFile',paramtyp='char*,HBITMAP,HDC,int,int',rettype='bool',fun=False)

'''
# a loop to show the nodes and attributes
for node,attrs in G.nodes(data=True):
    print(node,attrs)
'''
# add edges
G.add_edge('main','_access',desc='NULL',color='b',width='1.0')
G.add_edge('_access','_mkdir',desc='NULL',color='b',width='1.0')
G.add_edge('main','util.FindPath',desc='NULL',color='b',width='1.0')
G.add_edge('main','wstring',desc='NULL',color='b',width='1.0')
G.add_edge('main','CopyFile',desc='self-replicate',color='r',width='2.5')
G.add_edge('main','util.HideConsole',desc='avoid being detected',color='r',width='2.5')
G.add_edge('util.HideConsole','AllocConsole',desc='NULL',color='b',width='1.0')
G.add_edge('util.HideConsole','FindWindowA',desc='NULL',color='b',width='1.0')
G.add_edge('util.HideConsole','ShowWindow',desc='hide console',color='r',width='2.5')
G.add_edge('main','util.Autoload',desc='exe boots itself',color='r',width='2.5')
G.add_edge('util.Autoload','system',desc='write in registry',color='r',width='2.5')
G.add_edge('main','util.Screenshot',desc='get the screenshot',color='r',width='2.5')
G.add_edge('util.Screenshot','SaveBMPFile',desc='save the bmpbit',color='r',width='2.5')
G.add_edge('main','(thread logger(logger))logger.joinable',desc='judge if execute',color='r',width='2.5')
G.add_edge('(thread logger(logger))logger.joinable','(thread logger(logger))logger.join',desc='monitor the key stroke',color='r',width='2.5')
G.add_edge('(thread logger(logger))logger.join','SpecialKeys',desc='monitor the key stroke',color='r',width='2.5')
G.add_edge('SpecialKeys','WriteData',desc='write the key stroke into buffer',color='r',width='2.5')
G.add_edge('main','(thread screens(screens))screens.joinable',desc='judge if execute',color='r',width='2.5')
G.add_edge('(thread screens(screens))screens.joinable','(thread logger(screens))screens.join',desc='monitor the screen',color='r',width='2.5')
G.add_edge('(thread logger(screens))screens.join','util.Screenshot',desc='get the screenshot',color='r',width='2.5')

# generate all the nodes' attributes
node_labels={}
for node in G.nodes:
    node_labels[node]=G.nodes[node]

# generate edge_labels manually
edge_labels = {}
for edge in G.edges:
    edge_labels[edge] = G[edge[0]][edge[1]]

# set the attributes of nodes
pos=nx.arf_layout(G)

# draw the graph

# show the edges's attributes
# nx.draw_networkx_edge_labels(G,pos,edge_labels=edge_labels)


nx.draw(G
        ,pos
        ,with_labels=True
        ,edge_color=[edge_attrs['color'] for _,_,edge_attrs in G.edges(data=True)]
        )

plt.show()
# close the graph
G.clear()

