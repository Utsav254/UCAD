# UCAD

A personal project to learn the win32 api and direct X(D3D).
Initially, used to design a win32 api abstraction for easy of use and building the app framework.
Graphics and hardware acceleration through direct X integration.

Lastly use this to create a basic cad program.


### Ideas:
	
	1> model hierarchy: create tree view of all assemblies parts etc. 
						Subtrees with faces and edges etc.
						Should make use of this for hierachical bounding box frustum culling optimisation.
	
	2> frustum culling: event depedant checking of bounding boxes against frustum.
						also helps in detecting collision with near plane to turn back face culling off.
						Might not be required for all cases. Model depedant culling

	3> occlusion cullling: also model dependant and view mode dependant (xray view = no culling).
							Hierachical z buffer based occullusion culling.

	4> object picking optimisation: use a LRU cache for storing recently picked objects.
									Recently picked objects are likely to be picked again.
									Dont make this cache too big since looping through the cache would be expensive anyways.
									This might not be required with the uint32_t ID buffer plan.
									
									- is it faster to perform additional rendering pass and mapping unmapping that buffer.
									- vs
									- is it faster to intersection test objects in cache and then revert to ID buffer

									Double buffer the ID buffer using std::async futures and promises etc.
