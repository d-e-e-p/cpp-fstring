/**
 * @file class_include.h
 * misc demo of cpp-fstring
 *
 * @ingroup examples
 *
 * @author Sandeep M
 * @copyright Copyright 2023 Sandeep M<deep@tensorfield.ag>
 * @license MIT License
*/

/**
 * from:
 * https://github.com/robotpy/robotpy-cppheaderparser/blob/main/test/TestSampleClass.h
 * Copyright (C) 2011, Jashua R. Cloutier
 * License: BSD
 */

#include <vector>
#include <string>
#include <map>

#include "fstr.h"

using std::string;
using std::vector;

class BaseSampleClass {
	enum Elephant
	{
		EL_ONE = 1,
		EL_TWO = 2,
		EL_NINE = 9,
		EL_TEN,
	};
  Elephant el_base = EL_TWO ;
};

class SampleClass: public BaseSampleClass
{
    string prop1;
    int prop2;
    bool prop3;     /*!< prop6 description */
    double prop4;   //!< prop7 description
                    //!< with two lines

    /// prop8 description
    int prop5;
    Elephant el_derived = EL_TEN;
};
namespace Alpha
{
    class AlphaClass
    {
    public:
    	AlphaClass();

    	void alphaMethod();

      string alphaString;
    protected:
    	typedef enum
    	{
    		Z_A,
    		Z_B = 0x2B,
    		Z_C = 'j',
			Z_D,
         Z_E = '9',
         Z_F = 9,
    	} Zebra;
    };

    namespace Omega
    {
		class OmegaClass
		{
		public:
			OmegaClass();

			string omegaString;
		protected:
			///
			/// @brief Rino Numbers, not that that means anything
			///
			typedef enum
			{
				RI_ZERO, /// item zero
				RI_ONE,  /** item one */
				RI_TWO,   //!< item two
				RI_THREE,
				/// item four
				RI_FOUR,
			} Rino;
		};
    } // end namespace Omega
} // end namespace Alpha

//Sample class for bug 3488053
class Bug_3488053
{
	public:
	class Bug_3488053_Nested
	{
	public:
		int x;
	} nest;
};

// Bug 3488360
namespace Citrus
{
  class BloodOrange {
    public:
      int bo=1;
  };
}

class Bananna: public Citrus::BloodOrange
{
};

class ExcellentCake: private Citrus::BloodOrange
{
};

// Bug 3487551
class Bug_3487551
{
public:
    virtual int method();
};


// Feature Request 3488051
struct SampleStruct
{
    unsigned int meth();
private:
    int prop;
};

// Bug 3488049 & Feature Request 3488050
const int MAX_ITEM = 7;
class Bird
{
  public:
  int items[MAX_ITEM];
  int otherItems[7];
  int oneItem;
};

// Bug 3488054
class Monkey {
private:
static void Create();
};
inline void Monkey::Create() { }

// Bug 3488275
class Chicken
{
template <typename T> static T Get();
};
template <typename T> T Chicken::Get() { return T(); }

// Bug 3491240
class Lizzard {
Lizzard();
explicit Lizzard( int a );
};

// Bug 3491178
class Owl {
private:
template <typename T> int* tFunc( int count );
};

template <typename T> int* Owl::tFunc( int count ) {
if (count == 0) {
return NULL;
}
return NULL;
}

// Bug 3491232
class GrapeClass {
private:
struct GrapeStruct { };
int x;
void f();
};


// Bug 3491319
struct AnonHolderClass {
struct {
int x;
} a;
};


// Feature Request 3491220
class CowClass {};
struct CowStruct {};

// Bug 3491334
class BaseMangoClass { };
class MangoClass: virtual public BaseMangoClass { };

// Bug 3492237
const long MAX_LEN = 7;
struct EagleClass {
int a[(int)MAX_LEN];
};

// Bug 3497164
struct FrogClass {

    struct {
        int a;
    } x;

    struct {
        int b;
    } y;

    struct {
        int c;
    } z;

};

// Bug 3497160
class DogClass;
class CatClass {
    friend DogClass;
};

// Bug 3497155
extern "C" {
int FishA( const char* strA );
int FishB( const char* strB );
}

// Bug 3497168
class PandaClass {
static const int CONST_A = (1 << 7) - 1;
static const int CONST_B = sizeof(int);
};

// Bug 3497166
class PotatoClass {
    struct FwdStruct;
    FwdStruct* ptr;
    struct FwdStruct {
    	int a;
    };
};

// Bug 3497162
class HogClass {
	union HogUnion {
		int a;
		float b;
	} u;
};

// Bug 3497158
template< int ID >
class CherryClass {
public:
	CherryClass ();
	struct NestStruct {
	    void FuncA();
	    int val;
	};
};

// Bug 3517308
template<class T>
class GarlicClass
{
public:
	GarlicClass();
    int fun1(T);
    int fun2(T);
};

// Bug 3514728
class CarrotClass
{
int var1;
#define FIRSTLINE \
SECONDLINE
void fun1();
};

// Bug 3517289
extern "C" void f(int i, char c, float x);


// Bug 3514671
struct OliveStruct{
	struct other* a;
	void* b;
	bool c;
	int d;
};

// Bug 3515330
namespace RoosterNamespace
{
    class RoosterOuterClass
    {
    public:
        int member1;

        class RoosterSubClass1
        {
        public:
            int publicMember1;
        private:
            int privateMember1;
        };

    private:
        int member2;
        class RoosterSubClass2
        {
        public:
            int publicMember2;
        private:
            int privateMember2;
        };
    };
}


// Bug 3514672
class OperatorClass
{
public:
};

// Feature request 3519502 & 3523010
class CrowClass
{
public:
    int meth();

public:
    void testSlot(int i);
};

// Bug 3497170
struct DriverFuncs {
    void* (*init) ();
    void (*write) (void *buf, int buflen);
};


// Feature Request 3523235
class RockClass
{
    int getNum() const;
    int getNum2();
};


// Bug 3524327
class StoneClass
{
    virtual int getNum2() const = 0;
    int getNum3();
};

// Bug 3531219
class Kangaroo
{
public:
    Kangaroo() {
    }
    class Joey
    {
    public:
        Joey();
    };
};
