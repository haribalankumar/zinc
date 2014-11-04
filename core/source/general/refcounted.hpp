/**
 * FILE : general/refcounted.hpp
 * 
 * Base class for intrusively reference counted objects.
 */
/* OpenCMISS-Zinc Library
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#if !defined (CMZN_GENERAL_REFCOUNTED_HPP)
#define CMZN_GENERAL_REFCOUNTED_HPP

namespace cmzn
{

/**
 * Base class for intrusively reference counted objects.
 * Constructed on heap with refCount of 1.
 */
class RefCounted
{
	template<class REFCOUNTED>
		friend REFCOUNTED* ACCESS(REFCOUNTED* object);
	template<class REFCOUNTED>
		friend void DEACCESS(REFCOUNTED* &object);
	template<class REFCOUNTED>
		friend void REACCESS(REFCOUNTED* &object, REFCOUNTED* newObject);
protected:
	int access_count;

	RefCounted() :
		access_count(1)
	{
	}

	virtual ~RefCounted()
	{
	}

	inline void access()
	{
		++this->access_count;
	}

	void deaccess()
	{
		--this->access_count;
		if (this->access_count <= 0)
			delete this;
	}
};

template<class REFCOUNTED> inline REFCOUNTED* ACCESS(REFCOUNTED* object)
{
	if (object)
		object->access();
	return object;
}

template<class REFCOUNTED> inline void DEACCESS(REFCOUNTED* &object)
{
	if (object)
	{
		object->deaccess();
		object = 0;
	}
}

template<class REFCOUNTED> inline void REACCESS(REFCOUNTED* &object, REFCOUNTED* newObject)
{
	// access first to handle object==newObject
	if (newObject)
		newObject->access();
	DEACCESS(object);
	object = newObject;
}

}

#endif /* !defined (CMZN_GENERAL_REFCOUNTED_HPP) */
