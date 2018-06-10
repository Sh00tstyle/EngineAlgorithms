#ifndef OCTREE_H
#define OCTREE_H

class OctreeNode;

class Octree {
	public:
		Octree(float pHalfSize, float pHeight, int pDepth = 1);
		~Octree();

	private:
		OctreeNode * _childNodes[8]; //eight possible nodes

		float _halfSize;
		float _height;
		int _depth;

		void InitOctree();

};

#endif