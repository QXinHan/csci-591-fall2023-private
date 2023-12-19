import networkx as nx
import matplotlib.pyplot as plt

'''
so each node represents a function or a code module developed by the developer.
Each directed edge represents the relationship between two node,
such as, function A invokes function B and how it works. 
'''
search_path=[]

'''
# depth-first-search to search the malicious path and print it
# /param G:the directed graph
# /param V:the started node('main' function)
# /param edge_attribute:the special edges' attribute that needs to be paid attention
# /param edge_attribute_value:if the edge's attribute value is edge_attribute_value then it will be marked
'''
def travel(G,V,edge_attribute,edge_attribute_value):
    if(G.out_degree(V)==0):
        print({'name': 'main', 'paramtyp': 'void', 'rettype': 'int', 'fun': True})
        for i in search_path:
            print(G.nodes[i])
        print('----------------------------------------------')
        search_path.clear()
        return
    else:
        for node in G.neighbors(V):
            edge_data=G.get_edge_data(V,node)
            if G.has_edge(V,node) and edge_data.get(edge_attribute)==edge_attribute_value:
                search_path.append(node)
                travel(G,node,edge_attribute,edge_attribute_value)

# draw a directed graph to show the function invocation
G = nx.DiGraph()

# add nodes
# nodes' attributes are return value type, parameters type, function name
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
G.add_node(20,name='_mkdir',paramtyp='const char*',rettype='int',fun=True)
G.add_node(21,name='AllocConsole',paramtyp='void',rettype='void',fun=True)
G.add_node(22,name='FindWindowA',paramtyp='LPCSTR,LPCSTR',rettype='HANDLE',fun=True)
G.add_node(23,name='ShowWindow',paramtyp='HWND,int',rettype='BOOL',fun=True)
G.add_node(24,name='system',paramtyp='const char*',rettype='bool',fun=True)
G.add_node(25,name='GetFilePointer',paramtyp='HANDLE',rettype='int',fun=False)
G.add_node(26,name='setFilePointer',paramtyp='HANDLE,int,int,int',rettype='int',fun=True)
G.add_node(27,name='SaveBMPFile',paramtyp='char*,HBITMAP,HDC,int,int',rettype='bool',fun=False)

'''
# a loop to print the nodes and attributes
for node,attrs in G.nodes(data=True):
    print(node,attrs)
'''
# add edges
G.add_edge(0,19,desc='NULL',color='b',width='1.0')
G.add_edge(19,20,desc='NULL',color='b',width='1.0')
G.add_edge(0,10,desc='NULL',color='b',width='1.0')
G.add_edge(0,7,desc='NULL',color='b',width='1.0')
G.add_edge(0,5,desc='self-replicate',color='r',width='2.5')
G.add_edge(0,8,desc='avoid being detected',color='r',width='2.5')
G.add_edge(8,21,desc='NULL',color='b',width='1.0')
G.add_edge(8,22,desc='NULL',color='b',width='1.0')
G.add_edge(8,23,desc='hide console',color='r',width='2.5')
G.add_edge(0,9,desc='exe boots itself',color='r',width='2.5')
G.add_edge(9,24,desc='write in registry',color='r',width='2.5')
G.add_edge(0,13,desc='get the screenshot',color='r',width='2.5')
G.add_edge(13,27,desc='save the bmpbit',color='r',width='2.5')
G.add_edge(0,15,desc='judge if execute',color='r',width='2.5')
G.add_edge(15,16,desc='monitor the key stroke',color='r',width='2.5')
G.add_edge(16,2,desc='monitor the key stroke',color='r',width='2.5')
G.add_edge(2,1,desc='write the key stroke into buffer',color='r',width='2.5')
G.add_edge(0,17,desc='judge if execute',color='r',width='2.5')
G.add_edge(17,18,desc='monitor the screen',color='r',width='2.5')
G.add_edge(18,13,desc='get the screenshot',color='r',width='2.5')

'''
# generate all the nodes' attributes
node_labels={}
for node in G.nodes:
    node_labels[node]=G.nodes[node]

# generate edge_labels manually
edge_labels = {}
for edge in G.edges:
    edge_labels[edge] = G[edge[0]][edge[1]]
'''

#####
# To find the malicious paths and print them
travel(G,0,'color','r')
#####

# set the attributes of nodes
pos=nx.arf_layout(G)

# draw the graph
nx.draw(G
        ,pos
        ,with_labels=False
        ,edge_color=[edge_attrs['color'] for _,_,edge_attrs in G.edges(data=True)]
        )

# draw the node's name(function name)
node_labels=nx.get_node_attributes(G,'name')
nx.draw_networkx_labels(G,pos,labels=node_labels,font_size=10,alpha=0.7)

# show the graph
plt.show()

# close the graph
G.clear()
