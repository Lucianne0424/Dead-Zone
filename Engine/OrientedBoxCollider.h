#pragma once
#include "BaseCollider.h"

class OrientedBoxCollider : public BaseCollider
{
public:
	OrientedBoxCollider();
	virtual ~OrientedBoxCollider();

	void FinalUpdate();
	virtual bool Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance);
	virtual bool Intersects(shared_ptr<BoundingSphere> boundingSphere) override;
	virtual bool Intersects(shared_ptr<BoundingBox> boundingBox) override;
	virtual bool Intersects(shared_ptr<BoundingOrientedBox> boundingOrientedBox) override;

	shared_ptr<BoundingOrientedBox> GetBoundingOrientedBox() { return _boundingOrientedBox; }

private:
	shared_ptr<BoundingOrientedBox> _boundingOrientedBox;
};

