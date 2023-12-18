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
List_G_nodes = [
    'WinMain','StopAV','ShellExecute','do-while','randname'
    ,'FindFile','GetMail','SendMail','mSendMail','mFindNext','mReadMail',
                ]
G.add_nodes_from(List_G_nodes)

# add edges and attributes
G.add_edge('WinMain','randname',name='self-replicate')
G.add_edge('WinMain','StopAV',name='terminate malware')
G.add_edge('WinMain','ShellExecute',name='execute a script')
G.add_edge('WinMain','do-while',name='a loop to send mails')
G.add_edge('do-while','mReadMail',name='read the mails')
G.add_edge('do-while','mSendMail',name='send mails')
G.add_edge('do-while','mFindNext',name='enumerate the mails')
G.add_edge('WinMain','FindFile',name='find the address of contacts')
G.add_edge('FindFile','SendMail',name='send mails to others')
G.add_edge('SendMail','mSendMail',name='API:MAPISendMail')
G.add_edge('FindFile','GetMail',name='get the address of recipients')

# set the layout of the graph
pos = nx.arf_layout(G)
# draw the graph
nx.draw(G,pos,node_size=1200,with_labels=True,node_color='b',alpha=0.4,node_shape='s')
# set the font color
nx.draw_networkx_labels(G,pos,font_color='blue')
# set the edge labels
edge_labels = nx.get_edge_attributes(G,'name')
nx.draw_networkx_edge_labels(G,pos,edge_labels=edge_labels)
# show the directed graph
plt.show()
# close the graph
G.clear()

'''
note: because I have located the malicious code,
so this graph is just about the malicious code invocation relationship.
'''