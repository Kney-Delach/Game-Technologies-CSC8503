#include "Simplex.h"
#include "../../Common/Plane.h"
#include "Debug.h"
#include "../../Common/Maths.h"
using namespace NCL::Maths;

Simplex::Simplex()
{
	index = 0;
	size  = 0;
}

Simplex::~Simplex()
{
}

void Simplex::SetToTri(SupportPoint a, SupportPoint b, SupportPoint c) {
	verts[0]	= a;
	verts[1]	= b;
	verts[2]	= c;
	index		= 3;
	size		= 3;
}

void Simplex::SetToLine(SupportPoint a, SupportPoint b) {
	verts[0] = a;
	verts[1] = b;
	index	= 2;
	size	= 2;
}

void Simplex::Add(SupportPoint a) {
	verts[index]	= a;
	index			= (index + 1) % 4;
	size++;
}

void Simplex::RemoveOldestSupportPoint() {
	size--;
}

float Simplex::GetClosestDistance() {
	float distance = GetVertex(0).Length();

	for (int i = 1; i < size; ++i) {
		float tempDist = GetVertex(i).Length();
		distance = min(distance, tempDist);
	}
	return distance;
}

bool Simplex::ExtractCollisionInfo(Vector3& normal, float& penetration) {
	if (size != 4) {
		return false;
	}
	//determine tri with smallest projection to the origin

	Plane bestPlane;

	float bestDistance = FLT_MAX;

	int bestIndices[3];

	for (int i = 0; i < 4; ++i) {
		int ia = i;
		int ib = (i + 1) % 4; //getting the 'older' points on purpose
		int ic = (i + 2) % 4;

		Vector3 a = GetVertex(ia);
		Vector3 b = GetVertex(ib);
		Vector3 c = GetVertex(ic);

		Plane p = Plane::PlaneFromTri(a, b, c);

		float absDistance = abs(p.GetDistance());

		if (absDistance < bestDistance) {
			bestPlane = p;
			bestDistance = absDistance;
			bestIndices[0] = ia;
			bestIndices[1] = ib;
			bestIndices[2] = ic;
		}
	}
	//we now have the closest tri to the origin

	Vector3 planePoint = bestPlane.ProjectPointOntoPlane(Vector3());

	Vector3 a = GetVertex(bestIndices[0]);
	Vector3 b = GetVertex(bestIndices[1]);
	Vector3 c = GetVertex(bestIndices[2]);

	float triArea = abs(Maths::CrossAreaOfTri(a, b, c));

	float alpha = abs(Maths::CrossAreaOfTri(b, c, planePoint));
	float beta	= abs(Maths::CrossAreaOfTri(a, c, planePoint));
	float gamma = abs(Maths::CrossAreaOfTri(a, b, planePoint));

	float totalArea = 1.0f / triArea;

	alpha	= alpha * totalArea;
	beta	= beta * totalArea;
	gamma	= gamma * totalArea;

	float sum = alpha + beta + gamma;

	if (sum > 1.0001f || sum < 0.9999) {
		return false;
	}
	penetration = abs(bestPlane.GetDistance());
	normal		= bestPlane.GetNormal();

	return true;
}

//http://allenchou.net/2013/12/game-physics-collision-detection-gjk/
//http://box2d.org/files/GDC2010/GDC2010_Catto_Erin_GJK.pdf
//https://stackoverflow.com/questions/9605556/how-to-project-a-point-onto-a-plane-in-3d 
void Simplex::DetermineBestCollisions(Vector3& onA, Vector3& onB)	{
	if (size != 4) {
		return;
	}
	////there's 4 triangles in total in the simplex, which one is closest to the origin?
	int bestIndex = 0;

	Vector4 bestBarycentre;

	float bestBary = FLT_MAX;

	Vector4 barycentres[4];
	Vector4 points[4];

	Vector3 onAs[4];
	Vector3 onBs[4];

	Vector3 realAs[4];
	Vector3 realBs[4];

	Plane planes[4];

	for (int i = 0; i < 4; ++i) {
		int ia = i;
		int ib = (i + 1) % 4; //getting the 'older' points on purpose
		int ic = (i + 2) % 4;

		Vector3 a = GetVertex(ia);
		Vector3 b = GetVertex(ib);
		Vector3 c = GetVertex(ic);

		planes[i] = Plane::PlaneFromTri(a, b, c);

		Vector3 planePoint = planes[i].ProjectPointOntoPlane(Vector3());

		float triArea = abs(Maths::CrossAreaOfTri(a, b, c));

		float alpha = abs(Maths::CrossAreaOfTri(b, c, planePoint));
		float beta  = abs(Maths::CrossAreaOfTri(a, c, planePoint));
		float gamma = abs(Maths::CrossAreaOfTri(a, b, planePoint));

		int collisionOnPoint = -1;

		if (alpha < 0.0001f) {
			collisionOnPoint = 0;
		}
		if (beta < 0.0001f) {
			collisionOnPoint = 1;
		}
		if (gamma < 0.0001f) {
			collisionOnPoint = 2;
		}

		if (collisionOnPoint != -1) {
			//emergency! collision point is on a line
		}
		
		float totalArea = 1.0f / triArea;

		alpha = alpha * totalArea;
		beta  = beta  * totalArea;
		gamma = gamma * totalArea;

		float sum = alpha + beta + gamma;

		barycentres[i] = Vector4(alpha, beta, gamma, sum);

		Vector3 closestPoint = (a * alpha) + (b * beta) + (c * gamma);
		float l = closestPoint.Length();

		points[i] = Vector4(closestPoint.x, closestPoint.y, closestPoint.z, l);

		Vector3 closeA = GetSupportPoint(ia).onA *alpha;
		closeA += GetSupportPoint(ib).onA * beta;
		closeA += GetSupportPoint(ic).onA * gamma;

		onAs[i] = closeA;

		Vector3 closeB = GetSupportPoint(ia).onB *alpha;
		closeB += GetSupportPoint(ib).onB * beta;
		closeB += GetSupportPoint(ic).onB * gamma;

		onBs[i] = closeB;


		Vector3 realA = GetSupportPoint(ia).realA *alpha;
		realA += GetSupportPoint(ib).realA * beta;
		realA += GetSupportPoint(ic).realA * gamma;

		realAs[i] = realA;

		Vector3 realB = GetSupportPoint(ia).realB *alpha;
		realB += GetSupportPoint(ib).realB * beta;
		realB += GetSupportPoint(ic).realB * gamma;

		realBs[i] = realB;


		if (sum < bestBary) {
			bestBary = sum;
			bestIndex = i;

			onA = realA;
			onB = realB;

			bestBarycentre = barycentres[i];
		}
	}
	//try and find closest real pair...
	float testDist = FLT_MAX;

	float testDist2 = (onA - onB).Length();

	for (int i = 0; i < 4; ++i) {
		float d = (realAs[i] - realBs[i]).Length();
		if (d < testDist) {
			onA = realAs[i];
			onB = realBs[i];
			testDist = d;
		}
	}
	if (testDist < testDist2) {
		bool a = true;
	}

	if (testDist > 20.0f) {
		bool a = true;
	}
}


Vector3 Simplex::BarycentricTriangleWeighting(const Vector3& a, const Vector3&b, const Vector3&c) {
	Plane p = Plane::PlaneFromTri(a, b, c);

	Vector3 planePoint = p.ProjectPointOntoPlane(Vector3());

	float triArea	= abs(Maths::CrossAreaOfTri(a, b, c));

	float alpha		= abs(Maths::CrossAreaOfTri(b, c, planePoint));
	float beta		= abs(Maths::CrossAreaOfTri(a, c, planePoint));
	float gamma		= abs(Maths::CrossAreaOfTri(a, b, planePoint));

	float totalArea = 1.0f / triArea;

	alpha	= alpha * totalArea;
	beta	= beta  * totalArea;
	gamma	= gamma * totalArea;

	return Vector3(alpha, beta, gamma);
}

int Simplex::OriginInsideTetrahedron() {
	if (size != 4) {
		return 0;
	}

	int planeState = 0;

	for (int i = 0; i < 4; ++i) {
		int ia = i;
		int ib = (i + 1) % 4;
		int ic = (i + 2) % 4;
		int id = (i + 3) % 4;

		Vector3 a = GetVertex(ia);
		Vector3 b = GetVertex(ib);
		Vector3 c = GetVertex(ic);
		Vector3 d = GetVertex(id);

		Plane p = Plane::PlaneFromTri(a,b,c);

		float dDistance = p.DistanceFromPlane(d);

		if (dDistance > 0 && p.GetDistance() > 0 ||
			dDistance < 0 && p.GetDistance() < 0) {
			planeState |= 1 << i;
		}
		else {
			bool a = true;
		}
	}
	return planeState;
}

Vector3 Simplex::ClosestPointToTetrahedron(int planeStates, Vector4&interpolates) {
	if (size != 4) {
		return Vector3();
	}
	float bestDist = FLT_MAX;
	Vector3 currentBestPoint;

	for (int i = 0; i < 4; ++i) {
		if (planeStates & (1 << i)) {
			continue; //the origin was inside this point, skip it
		}
		int ia = i;
		int ib = (i + 1) % 4; //getting the 'older' points on purpose
		int ic = (i + 2) % 4;

		Vector3 a = GetVertex(ia);
		Vector3 b = GetVertex(ib);
		Vector3 c = GetVertex(ic);

		Vector3 barycentrics = closestPointToTri(a, b, c);

		Vector3 closestPoint = a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;

		if (closestPoint.Length() < bestDist) {
			currentBestPoint = closestPoint;

			bestDist = closestPoint.Length();

			interpolates = Vector4(0, 0, 0, 0);
			interpolates.array[ia] = barycentrics.x;
			interpolates.array[ib] = barycentrics.y;
			interpolates.array[ic] = barycentrics.z;
		}
	}
	return currentBestPoint;
}

//https://slideplayer.com/slide/689954/
Vector3 Simplex::closestPointToTri(const Vector3& a, const Vector3&b, const Vector3& c) {
	//need to take into consideration the voronoi regions of the vertices and edges!

	Vector3 ab = b - a;
	Vector3 ac = c - a;

	float aAB = Vector3::Dot(ab, -a);
	float aAC = Vector3::Dot(ac, -a);

	if (aAB < 0.0f && aAC < 0.0f) { //origin is in voronoi region of vertex A
		return Vector3(1, 0, 0);
	}

	float bAB = Vector3::Dot(ab, -b);
	float bAC = Vector3::Dot(ac, -b);

	if (bAB >= 0.0f && bAC <= bAB) { //origin is in voronoi region of vertex B
		return Vector3(0, 1, 0);
	}

	float cAB = Vector3::Dot(ab, -c);
	float cAC = Vector3::Dot(ac, -c);

	if (cAC > 0.0f && cAB <= cAC) { //origin is in voronoi region of vertex C
		return Vector3(0, 0, 1);
	}

	//form 3 'edge planes' (it's a magic toblerone
	
	Plane p				= Plane::PlaneFromTri(a, b, c);
	Vector3 planePoint  = p.ProjectPointOntoPlane(Vector3());

	Plane pBA = Plane::PlaneFromTri(a, b, b + p.GetNormal());
	Plane pBC = Plane::PlaneFromTri(b, c, c + p.GetNormal());
	Plane pCA = Plane::PlaneFromTri(c, a, a + p.GetNormal());

	if (pBA.GetDistance() < 0 && pBC.GetDistance() < 0 && pCA.GetDistance() < 0) {
		//origin is inside this triangle! do the triangle projection

		Vector3 barycentrics = BarycentricTriangleWeighting(a, b, c);

		return barycentrics;
	}

	if (pBA.GetDistance() > 0) { //outside of edge AB
		Vector3 edgePoint = pBA.ProjectPointOntoPlane(planePoint); //point along AB

		float bLength = (edgePoint - b).Length();
		float aLength = (b - a).Length();

		float ratio = bLength / aLength;
		
		Vector3 barycentrics(1.0f - ratio, ratio, 0);
		return barycentrics;
	}

	if (pBC.GetDistance() > 0) { //outside of edge BC
		Vector3 edgePoint = pBC.ProjectPointOntoPlane(planePoint); //point along BC

		float bLength = (edgePoint - c).Length();
		float aLength = (c - b).Length();

		float ratio = bLength / aLength;

		Vector3 barycentrics(0.0f, 1.0f - ratio, ratio);
		return barycentrics;
	}

	if (pCA.GetDistance() > 0) { //outside of edge AC
		Vector3 edgePoint = pCA.ProjectPointOntoPlane(planePoint); //point along AC

		float bLength = (edgePoint - a).Length();
		float aLength = (a - c).Length();

		float ratio = bLength / aLength;

		Vector3 barycentrics(ratio, 0, 1.0f - ratio);
		return barycentrics;
	}

	bool wtf = true;

	return Vector3();
}

Vector3 Simplex::ClosestPoint() {
	if (size == 1) {
		return GetVertex(0);
	}

	if (size == 2) {
		//find point on line closest to origin
		return Vector3();
	}

	if (size == 3) {
		Vector3 a = GetVertex(0);
		Vector3 b = GetVertex(1);
		Vector3 c = GetVertex(2);

		Vector3 barycentric = closestPointToTri(a, b, c);

		return a * barycentric.x + b * barycentric.y + c * barycentric.z;
	}

	if (size == 4) {

	}

	return Vector3();
}