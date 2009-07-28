/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsall.idl
 */

#ifndef __gen_nsall_h__
#define __gen_nsall_h__
/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
#if 0
typedef PRUint32 nsresult;

typedef PRUint32 nsrefcnt;

typedef PRUint32 PRUint32;

typedef PRInt32 PRInt32;

typedef PRInt64 PRInt64;

typedef PRUint64 PRTime;

#endif

/* starting interface:    nsISupports */
#define NS_ISUPPORTS_IID_STR "00000000-0000-0000-c000-000000000046"

#define NS_ISUPPORTS_IID \
  {0x00000000, 0x0000, 0x0000, \
    { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 }}

class NS_NO_VTABLE nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISUPPORTS_IID)

  /* void QueryInterface (in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD QueryInterface(const nsIID & iid, void * *result) = 0;

  /* [noscript, notxpcom] nsrefcnt AddRef (); */
  NS_IMETHOD_(nsrefcnt) AddRef(void) = 0;

  /* [noscript, notxpcom] nsrefcnt Release (); */
  NS_IMETHOD_(nsrefcnt) Release(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISUPPORTS \
  NS_IMETHOD QueryInterface(const nsIID & iid, void * *result); \
  NS_IMETHOD_(nsrefcnt) AddRef(void); \
  NS_IMETHOD_(nsrefcnt) Release(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISUPPORTS(_to) \
  NS_IMETHOD QueryInterface(const nsIID & iid, void * *result) { return _to QueryInterface(iid, result); } \
  NS_IMETHOD_(nsrefcnt) AddRef(void) { return _to AddRef(); } \
  NS_IMETHOD_(nsrefcnt) Release(void) { return _to Release(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISUPPORTS(_to) \
  NS_IMETHOD QueryInterface(const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->QueryInterface(iid, result); } \
  NS_IMETHOD_(nsrefcnt) AddRef(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->AddRef(); } \
  NS_IMETHOD_(nsrefcnt) Release(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Release(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsSupports : public nsISupports
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISUPPORTS

  nsSupports();

private:
  ~nsSupports();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsSupports, nsISupports)

nsSupports::nsSupports()
{
  /* member initializers and constructor code */
}

nsSupports::~nsSupports()
{
  /* destructor code */
}

/* void QueryInterface (in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsSupports::QueryInterface(const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript, notxpcom] nsrefcnt AddRef (); */
NS_IMETHODIMP_(nsrefcnt) nsSupports::AddRef()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript, notxpcom] nsrefcnt Release (); */
NS_IMETHODIMP_(nsrefcnt) nsSupports::Release()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsISimpleEnumerator */
#define NS_ISIMPLEENUMERATOR_IID_STR "d1899240-f9d2-11d2-bdd6-000064657374"

#define NS_ISIMPLEENUMERATOR_IID \
  {0xd1899240, 0xf9d2, 0x11d2, \
    { 0xbd, 0xd6, 0x00, 0x00, 0x64, 0x65, 0x73, 0x74 }}

class NS_NO_VTABLE nsISimpleEnumerator : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISIMPLEENUMERATOR_IID)

  /* boolean hasMoreElements (); */
  NS_IMETHOD HasMoreElements(PRBool *_retval) = 0;

  /* nsISupports getNext (); */
  NS_IMETHOD GetNext(nsISupports **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISIMPLEENUMERATOR \
  NS_IMETHOD HasMoreElements(PRBool *_retval); \
  NS_IMETHOD GetNext(nsISupports **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISIMPLEENUMERATOR(_to) \
  NS_IMETHOD HasMoreElements(PRBool *_retval) { return _to HasMoreElements(_retval); } \
  NS_IMETHOD GetNext(nsISupports **_retval) { return _to GetNext(_retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISIMPLEENUMERATOR(_to) \
  NS_IMETHOD HasMoreElements(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->HasMoreElements(_retval); } \
  NS_IMETHOD GetNext(nsISupports **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNext(_retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsSimpleEnumerator : public nsISimpleEnumerator
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISIMPLEENUMERATOR

  nsSimpleEnumerator();

private:
  ~nsSimpleEnumerator();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsSimpleEnumerator, nsISimpleEnumerator)

nsSimpleEnumerator::nsSimpleEnumerator()
{
  /* member initializers and constructor code */
}

nsSimpleEnumerator::~nsSimpleEnumerator()
{
  /* destructor code */
}

/* boolean hasMoreElements (); */
NS_IMETHODIMP nsSimpleEnumerator::HasMoreElements(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsISupports getNext (); */
NS_IMETHODIMP nsSimpleEnumerator::GetNext(nsISupports **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIFactory */
#define NS_IFACTORY_IID_STR "00000001-0000-0000-c000-000000000046"

#define NS_IFACTORY_IID \
  {0x00000001, 0x0000, 0x0000, \
    { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 }}

class NS_NO_VTABLE nsIFactory : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IFACTORY_IID)

  /* void createInstance (in nsISupports outer, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD CreateInstance(nsISupports *outer, const nsIID & iid, void * *result) = 0;

  /* void lockFactory (in boolean lock); */
  NS_IMETHOD LockFactory(PRBool lock) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIFACTORY \
  NS_IMETHOD CreateInstance(nsISupports *outer, const nsIID & iid, void * *result); \
  NS_IMETHOD LockFactory(PRBool lock); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIFACTORY(_to) \
  NS_IMETHOD CreateInstance(nsISupports *outer, const nsIID & iid, void * *result) { return _to CreateInstance(outer, iid, result); } \
  NS_IMETHOD LockFactory(PRBool lock) { return _to LockFactory(lock); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIFACTORY(_to) \
  NS_IMETHOD CreateInstance(nsISupports *outer, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateInstance(outer, iid, result); } \
  NS_IMETHOD LockFactory(PRBool lock) { return !_to ? NS_ERROR_NULL_POINTER : _to->LockFactory(lock); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsFactory : public nsIFactory
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIFACTORY

  nsFactory();

private:
  ~nsFactory();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsFactory, nsIFactory)

nsFactory::nsFactory()
{
  /* member initializers and constructor code */
}

nsFactory::~nsFactory()
{
  /* destructor code */
}

/* void createInstance (in nsISupports outer, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsFactory::CreateInstance(nsISupports *outer, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void lockFactory (in boolean lock); */
NS_IMETHODIMP nsFactory::LockFactory(PRBool lock)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIInterfaceRequestor */
#define NS_IINTERFACEREQUESTOR_IID_STR "033a1470-8b2a-11d3-af88-00a024ffc08c"

#define NS_IINTERFACEREQUESTOR_IID \
  {0x033a1470, 0x8b2a, 0x11d3, \
    { 0xaf, 0x88, 0x00, 0xa0, 0x24, 0xff, 0xc0, 0x8c }}

class NS_NO_VTABLE nsIInterfaceRequestor : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IINTERFACEREQUESTOR_IID)

  /* void getInterface (in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD GetInterface(const nsIID & iid, void * *result) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIINTERFACEREQUESTOR \
  NS_IMETHOD GetInterface(const nsIID & iid, void * *result); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIINTERFACEREQUESTOR(_to) \
  NS_IMETHOD GetInterface(const nsIID & iid, void * *result) { return _to GetInterface(iid, result); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIINTERFACEREQUESTOR(_to) \
  NS_IMETHOD GetInterface(const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetInterface(iid, result); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsInterfaceRequestor : public nsIInterfaceRequestor
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIINTERFACEREQUESTOR

  nsInterfaceRequestor();

private:
  ~nsInterfaceRequestor();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsInterfaceRequestor, nsIInterfaceRequestor)

nsInterfaceRequestor::nsInterfaceRequestor()
{
  /* member initializers and constructor code */
}

nsInterfaceRequestor::~nsInterfaceRequestor()
{
  /* destructor code */
}

/* void getInterface (in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsInterfaceRequestor::GetInterface(const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIWeakReference */
#define NS_IWEAKREFERENCE_IID_STR "9188bc85-f92e-11d2-81ef-0060083a0bcf"

#define NS_IWEAKREFERENCE_IID \
  {0x9188bc85, 0xf92e, 0x11d2, \
    { 0x81, 0xef, 0x00, 0x60, 0x08, 0x3a, 0x0b, 0xcf }}

class NS_NO_VTABLE nsIWeakReference : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEAKREFERENCE_IID)

  /* void QueryReferent (in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD QueryReferent(const nsIID & iid, void * *result) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEAKREFERENCE \
  NS_IMETHOD QueryReferent(const nsIID & iid, void * *result); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEAKREFERENCE(_to) \
  NS_IMETHOD QueryReferent(const nsIID & iid, void * *result) { return _to QueryReferent(iid, result); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEAKREFERENCE(_to) \
  NS_IMETHOD QueryReferent(const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->QueryReferent(iid, result); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWeakReference : public nsIWeakReference
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEAKREFERENCE

  nsWeakReference();

private:
  ~nsWeakReference();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWeakReference, nsIWeakReference)

nsWeakReference::nsWeakReference()
{
  /* member initializers and constructor code */
}

nsWeakReference::~nsWeakReference()
{
  /* destructor code */
}

/* void QueryReferent (in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsWeakReference::QueryReferent(const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsISupportsWeakReference */
#define NS_ISUPPORTSWEAKREFERENCE_IID_STR "9188bc86-f92e-11d2-81ef-0060083a0bcf"

#define NS_ISUPPORTSWEAKREFERENCE_IID \
  {0x9188bc86, 0xf92e, 0x11d2, \
    { 0x81, 0xef, 0x00, 0x60, 0x08, 0x3a, 0x0b, 0xcf }}

class NS_NO_VTABLE nsISupportsWeakReference : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISUPPORTSWEAKREFERENCE_IID)

  /* nsIWeakReference GetWeakReference (); */
  NS_IMETHOD GetWeakReference(nsIWeakReference **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISUPPORTSWEAKREFERENCE \
  NS_IMETHOD GetWeakReference(nsIWeakReference **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISUPPORTSWEAKREFERENCE(_to) \
  NS_IMETHOD GetWeakReference(nsIWeakReference **_retval) { return _to GetWeakReference(_retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISUPPORTSWEAKREFERENCE(_to) \
  NS_IMETHOD GetWeakReference(nsIWeakReference **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWeakReference(_retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsSupportsWeakReference : public nsISupportsWeakReference
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISUPPORTSWEAKREFERENCE

  nsSupportsWeakReference();

private:
  ~nsSupportsWeakReference();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsSupportsWeakReference, nsISupportsWeakReference)

nsSupportsWeakReference::nsSupportsWeakReference()
{
  /* member initializers and constructor code */
}

nsSupportsWeakReference::~nsSupportsWeakReference()
{
  /* destructor code */
}

/* nsIWeakReference GetWeakReference (); */
NS_IMETHODIMP nsSupportsWeakReference::GetWeakReference(nsIWeakReference **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsISimpleEnumerator; /* forward declaration */


/* starting interface:    nsIFile */
#define NS_IFILE_IID_STR "c8c0a080-0868-11d3-915f-d9d889d48e3c"

#define NS_IFILE_IID \
  {0xc8c0a080, 0x0868, 0x11d3, \
    { 0x91, 0x5f, 0xd9, 0xd8, 0x89, 0xd4, 0x8e, 0x3c }}

class NS_NO_VTABLE nsIFile : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IFILE_IID)

  enum { NORMAL_FILE_TYPE = 0U };

  enum { DIRECTORY_TYPE = 1U };

  /* void append (in AString node); */
  NS_IMETHOD Append(const nsAString & node) = 0;

  /* [noscript] void appendNative (in ACString node); */
  NS_IMETHOD AppendNative(const nsACString & node) = 0;

  /* void normalize (); */
  NS_IMETHOD Normalize(void) = 0;

  /* void create (in PRUint32 type, in PRUint32 permissions); */
  NS_IMETHOD Create(PRUint32 type, PRUint32 permissions) = 0;

  /* attribute AString leafName; */
  NS_IMETHOD GetLeafName(nsAString & aLeafName) = 0;
  NS_IMETHOD SetLeafName(const nsAString & aLeafName) = 0;

  /* [noscript] attribute ACString nativeLeafName; */
  NS_IMETHOD GetNativeLeafName(nsACString & aNativeLeafName) = 0;
  NS_IMETHOD SetNativeLeafName(const nsACString & aNativeLeafName) = 0;

  /* void copyTo (in nsIFile parent_dir, in AString name); */
  NS_IMETHOD CopyTo(nsIFile *parent_dir, const nsAString & name) = 0;

  /* [noscript] void copyToNative (in nsIFile parent_dir, in ACString name); */
  NS_IMETHOD CopyToNative(nsIFile *parent_dir, const nsACString & name) = 0;

  /* void copyToFollowingLinks (in nsIFile parent_dir, in AString name); */
  NS_IMETHOD CopyToFollowingLinks(nsIFile *parent_dir, const nsAString & name) = 0;

  /* [noscript] void copyToFollowingLinksNative (in nsIFile parent_dir, in ACString name); */
  NS_IMETHOD CopyToFollowingLinksNative(nsIFile *parent_dir, const nsACString & name) = 0;

  /* void moveTo (in nsIFile parent_dir, in AString name); */
  NS_IMETHOD MoveTo(nsIFile *parent_dir, const nsAString & name) = 0;

  /* [noscript] void moveToNative (in nsIFile parent_dir, in ACString name); */
  NS_IMETHOD MoveToNative(nsIFile *parent_dir, const nsACString & name) = 0;

  /* void remove (in boolean recursive); */
  NS_IMETHOD Remove(PRBool recursive) = 0;

  /* attribute unsigned long permissions; */
  NS_IMETHOD GetPermissions(PRUint32 *aPermissions) = 0;
  NS_IMETHOD SetPermissions(PRUint32 aPermissions) = 0;

  /* attribute unsigned long permissionsOfLink; */
  NS_IMETHOD GetPermissionsOfLink(PRUint32 *aPermissionsOfLink) = 0;
  NS_IMETHOD SetPermissionsOfLink(PRUint32 aPermissionsOfLink) = 0;

  /* attribute PRInt64 lastModifiedTime; */
  NS_IMETHOD GetLastModifiedTime(PRInt64 *aLastModifiedTime) = 0;
  NS_IMETHOD SetLastModifiedTime(PRInt64 aLastModifiedTime) = 0;

  /* attribute PRInt64 lastModifiedTimeOfLink; */
  NS_IMETHOD GetLastModifiedTimeOfLink(PRInt64 *aLastModifiedTimeOfLink) = 0;
  NS_IMETHOD SetLastModifiedTimeOfLink(PRInt64 aLastModifiedTimeOfLink) = 0;

  /* attribute PRInt64 fileSize; */
  NS_IMETHOD GetFileSize(PRInt64 *aFileSize) = 0;
  NS_IMETHOD SetFileSize(PRInt64 aFileSize) = 0;

  /* readonly attribute PRInt64 fileSizeOfLink; */
  NS_IMETHOD GetFileSizeOfLink(PRInt64 *aFileSizeOfLink) = 0;

  /* readonly attribute AString target; */
  NS_IMETHOD GetTarget(nsAString & aTarget) = 0;

  /* [noscript] readonly attribute ACString nativeTarget; */
  NS_IMETHOD GetNativeTarget(nsACString & aNativeTarget) = 0;

  /* readonly attribute AString path; */
  NS_IMETHOD GetPath(nsAString & aPath) = 0;

  /* [noscript] readonly attribute ACString nativePath; */
  NS_IMETHOD GetNativePath(nsACString & aNativePath) = 0;

  /* boolean exists (); */
  NS_IMETHOD Exists(PRBool *_retval) = 0;

  /* boolean isWritable (); */
  NS_IMETHOD IsWritable(PRBool *_retval) = 0;

  /* boolean isReadable (); */
  NS_IMETHOD IsReadable(PRBool *_retval) = 0;

  /* boolean isExecutable (); */
  NS_IMETHOD IsExecutable(PRBool *_retval) = 0;

  /* boolean isHidden (); */
  NS_IMETHOD IsHidden(PRBool *_retval) = 0;

  /* boolean isDirectory (); */
  NS_IMETHOD IsDirectory(PRBool *_retval) = 0;

  /* boolean isFile (); */
  NS_IMETHOD IsFile(PRBool *_retval) = 0;

  /* boolean isSymlink (); */
  NS_IMETHOD IsSymlink(PRBool *_retval) = 0;

  /* boolean isSpecial (); */
  NS_IMETHOD IsSpecial(PRBool *_retval) = 0;

  /* void createUnique (in PRUint32 type, in PRUint32 permissions); */
  NS_IMETHOD CreateUnique(PRUint32 type, PRUint32 permissions) = 0;

  /* nsIFile clone (); */
  NS_IMETHOD Clone(nsIFile **_retval) = 0;

  /* boolean equals (in nsIFile file); */
  NS_IMETHOD Equals(nsIFile *file, PRBool *_retval) = 0;

  /* boolean contains (in nsIFile file, in boolean recurse); */
  NS_IMETHOD Contains(nsIFile *file, PRBool recurse, PRBool *_retval) = 0;

  /* readonly attribute nsIFile parent; */
  NS_IMETHOD GetParent(nsIFile * *aParent) = 0;

  /* readonly attribute nsISimpleEnumerator directoryEntries; */
  NS_IMETHOD GetDirectoryEntries(nsISimpleEnumerator * *aDirectoryEntries) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIFILE \
  NS_IMETHOD Append(const nsAString & node); \
  NS_IMETHOD AppendNative(const nsACString & node); \
  NS_IMETHOD Normalize(void); \
  NS_IMETHOD Create(PRUint32 type, PRUint32 permissions); \
  NS_IMETHOD GetLeafName(nsAString & aLeafName); \
  NS_IMETHOD SetLeafName(const nsAString & aLeafName); \
  NS_IMETHOD GetNativeLeafName(nsACString & aNativeLeafName); \
  NS_IMETHOD SetNativeLeafName(const nsACString & aNativeLeafName); \
  NS_IMETHOD CopyTo(nsIFile *parent_dir, const nsAString & name); \
  NS_IMETHOD CopyToNative(nsIFile *parent_dir, const nsACString & name); \
  NS_IMETHOD CopyToFollowingLinks(nsIFile *parent_dir, const nsAString & name); \
  NS_IMETHOD CopyToFollowingLinksNative(nsIFile *parent_dir, const nsACString & name); \
  NS_IMETHOD MoveTo(nsIFile *parent_dir, const nsAString & name); \
  NS_IMETHOD MoveToNative(nsIFile *parent_dir, const nsACString & name); \
  NS_IMETHOD Remove(PRBool recursive); \
  NS_IMETHOD GetPermissions(PRUint32 *aPermissions); \
  NS_IMETHOD SetPermissions(PRUint32 aPermissions); \
  NS_IMETHOD GetPermissionsOfLink(PRUint32 *aPermissionsOfLink); \
  NS_IMETHOD SetPermissionsOfLink(PRUint32 aPermissionsOfLink); \
  NS_IMETHOD GetLastModifiedTime(PRInt64 *aLastModifiedTime); \
  NS_IMETHOD SetLastModifiedTime(PRInt64 aLastModifiedTime); \
  NS_IMETHOD GetLastModifiedTimeOfLink(PRInt64 *aLastModifiedTimeOfLink); \
  NS_IMETHOD SetLastModifiedTimeOfLink(PRInt64 aLastModifiedTimeOfLink); \
  NS_IMETHOD GetFileSize(PRInt64 *aFileSize); \
  NS_IMETHOD SetFileSize(PRInt64 aFileSize); \
  NS_IMETHOD GetFileSizeOfLink(PRInt64 *aFileSizeOfLink); \
  NS_IMETHOD GetTarget(nsAString & aTarget); \
  NS_IMETHOD GetNativeTarget(nsACString & aNativeTarget); \
  NS_IMETHOD GetPath(nsAString & aPath); \
  NS_IMETHOD GetNativePath(nsACString & aNativePath); \
  NS_IMETHOD Exists(PRBool *_retval); \
  NS_IMETHOD IsWritable(PRBool *_retval); \
  NS_IMETHOD IsReadable(PRBool *_retval); \
  NS_IMETHOD IsExecutable(PRBool *_retval); \
  NS_IMETHOD IsHidden(PRBool *_retval); \
  NS_IMETHOD IsDirectory(PRBool *_retval); \
  NS_IMETHOD IsFile(PRBool *_retval); \
  NS_IMETHOD IsSymlink(PRBool *_retval); \
  NS_IMETHOD IsSpecial(PRBool *_retval); \
  NS_IMETHOD CreateUnique(PRUint32 type, PRUint32 permissions); \
  NS_IMETHOD Clone(nsIFile **_retval); \
  NS_IMETHOD Equals(nsIFile *file, PRBool *_retval); \
  NS_IMETHOD Contains(nsIFile *file, PRBool recurse, PRBool *_retval); \
  NS_IMETHOD GetParent(nsIFile * *aParent); \
  NS_IMETHOD GetDirectoryEntries(nsISimpleEnumerator * *aDirectoryEntries); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIFILE(_to) \
  NS_IMETHOD Append(const nsAString & node) { return _to Append(node); } \
  NS_IMETHOD AppendNative(const nsACString & node) { return _to AppendNative(node); } \
  NS_IMETHOD Normalize(void) { return _to Normalize(); } \
  NS_IMETHOD Create(PRUint32 type, PRUint32 permissions) { return _to Create(type, permissions); } \
  NS_IMETHOD GetLeafName(nsAString & aLeafName) { return _to GetLeafName(aLeafName); } \
  NS_IMETHOD SetLeafName(const nsAString & aLeafName) { return _to SetLeafName(aLeafName); } \
  NS_IMETHOD GetNativeLeafName(nsACString & aNativeLeafName) { return _to GetNativeLeafName(aNativeLeafName); } \
  NS_IMETHOD SetNativeLeafName(const nsACString & aNativeLeafName) { return _to SetNativeLeafName(aNativeLeafName); } \
  NS_IMETHOD CopyTo(nsIFile *parent_dir, const nsAString & name) { return _to CopyTo(parent_dir, name); } \
  NS_IMETHOD CopyToNative(nsIFile *parent_dir, const nsACString & name) { return _to CopyToNative(parent_dir, name); } \
  NS_IMETHOD CopyToFollowingLinks(nsIFile *parent_dir, const nsAString & name) { return _to CopyToFollowingLinks(parent_dir, name); } \
  NS_IMETHOD CopyToFollowingLinksNative(nsIFile *parent_dir, const nsACString & name) { return _to CopyToFollowingLinksNative(parent_dir, name); } \
  NS_IMETHOD MoveTo(nsIFile *parent_dir, const nsAString & name) { return _to MoveTo(parent_dir, name); } \
  NS_IMETHOD MoveToNative(nsIFile *parent_dir, const nsACString & name) { return _to MoveToNative(parent_dir, name); } \
  NS_IMETHOD Remove(PRBool recursive) { return _to Remove(recursive); } \
  NS_IMETHOD GetPermissions(PRUint32 *aPermissions) { return _to GetPermissions(aPermissions); } \
  NS_IMETHOD SetPermissions(PRUint32 aPermissions) { return _to SetPermissions(aPermissions); } \
  NS_IMETHOD GetPermissionsOfLink(PRUint32 *aPermissionsOfLink) { return _to GetPermissionsOfLink(aPermissionsOfLink); } \
  NS_IMETHOD SetPermissionsOfLink(PRUint32 aPermissionsOfLink) { return _to SetPermissionsOfLink(aPermissionsOfLink); } \
  NS_IMETHOD GetLastModifiedTime(PRInt64 *aLastModifiedTime) { return _to GetLastModifiedTime(aLastModifiedTime); } \
  NS_IMETHOD SetLastModifiedTime(PRInt64 aLastModifiedTime) { return _to SetLastModifiedTime(aLastModifiedTime); } \
  NS_IMETHOD GetLastModifiedTimeOfLink(PRInt64 *aLastModifiedTimeOfLink) { return _to GetLastModifiedTimeOfLink(aLastModifiedTimeOfLink); } \
  NS_IMETHOD SetLastModifiedTimeOfLink(PRInt64 aLastModifiedTimeOfLink) { return _to SetLastModifiedTimeOfLink(aLastModifiedTimeOfLink); } \
  NS_IMETHOD GetFileSize(PRInt64 *aFileSize) { return _to GetFileSize(aFileSize); } \
  NS_IMETHOD SetFileSize(PRInt64 aFileSize) { return _to SetFileSize(aFileSize); } \
  NS_IMETHOD GetFileSizeOfLink(PRInt64 *aFileSizeOfLink) { return _to GetFileSizeOfLink(aFileSizeOfLink); } \
  NS_IMETHOD GetTarget(nsAString & aTarget) { return _to GetTarget(aTarget); } \
  NS_IMETHOD GetNativeTarget(nsACString & aNativeTarget) { return _to GetNativeTarget(aNativeTarget); } \
  NS_IMETHOD GetPath(nsAString & aPath) { return _to GetPath(aPath); } \
  NS_IMETHOD GetNativePath(nsACString & aNativePath) { return _to GetNativePath(aNativePath); } \
  NS_IMETHOD Exists(PRBool *_retval) { return _to Exists(_retval); } \
  NS_IMETHOD IsWritable(PRBool *_retval) { return _to IsWritable(_retval); } \
  NS_IMETHOD IsReadable(PRBool *_retval) { return _to IsReadable(_retval); } \
  NS_IMETHOD IsExecutable(PRBool *_retval) { return _to IsExecutable(_retval); } \
  NS_IMETHOD IsHidden(PRBool *_retval) { return _to IsHidden(_retval); } \
  NS_IMETHOD IsDirectory(PRBool *_retval) { return _to IsDirectory(_retval); } \
  NS_IMETHOD IsFile(PRBool *_retval) { return _to IsFile(_retval); } \
  NS_IMETHOD IsSymlink(PRBool *_retval) { return _to IsSymlink(_retval); } \
  NS_IMETHOD IsSpecial(PRBool *_retval) { return _to IsSpecial(_retval); } \
  NS_IMETHOD CreateUnique(PRUint32 type, PRUint32 permissions) { return _to CreateUnique(type, permissions); } \
  NS_IMETHOD Clone(nsIFile **_retval) { return _to Clone(_retval); } \
  NS_IMETHOD Equals(nsIFile *file, PRBool *_retval) { return _to Equals(file, _retval); } \
  NS_IMETHOD Contains(nsIFile *file, PRBool recurse, PRBool *_retval) { return _to Contains(file, recurse, _retval); } \
  NS_IMETHOD GetParent(nsIFile * *aParent) { return _to GetParent(aParent); } \
  NS_IMETHOD GetDirectoryEntries(nsISimpleEnumerator * *aDirectoryEntries) { return _to GetDirectoryEntries(aDirectoryEntries); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIFILE(_to) \
  NS_IMETHOD Append(const nsAString & node) { return !_to ? NS_ERROR_NULL_POINTER : _to->Append(node); } \
  NS_IMETHOD AppendNative(const nsACString & node) { return !_to ? NS_ERROR_NULL_POINTER : _to->AppendNative(node); } \
  NS_IMETHOD Normalize(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Normalize(); } \
  NS_IMETHOD Create(PRUint32 type, PRUint32 permissions) { return !_to ? NS_ERROR_NULL_POINTER : _to->Create(type, permissions); } \
  NS_IMETHOD GetLeafName(nsAString & aLeafName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLeafName(aLeafName); } \
  NS_IMETHOD SetLeafName(const nsAString & aLeafName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLeafName(aLeafName); } \
  NS_IMETHOD GetNativeLeafName(nsACString & aNativeLeafName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNativeLeafName(aNativeLeafName); } \
  NS_IMETHOD SetNativeLeafName(const nsACString & aNativeLeafName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetNativeLeafName(aNativeLeafName); } \
  NS_IMETHOD CopyTo(nsIFile *parent_dir, const nsAString & name) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyTo(parent_dir, name); } \
  NS_IMETHOD CopyToNative(nsIFile *parent_dir, const nsACString & name) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyToNative(parent_dir, name); } \
  NS_IMETHOD CopyToFollowingLinks(nsIFile *parent_dir, const nsAString & name) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyToFollowingLinks(parent_dir, name); } \
  NS_IMETHOD CopyToFollowingLinksNative(nsIFile *parent_dir, const nsACString & name) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyToFollowingLinksNative(parent_dir, name); } \
  NS_IMETHOD MoveTo(nsIFile *parent_dir, const nsAString & name) { return !_to ? NS_ERROR_NULL_POINTER : _to->MoveTo(parent_dir, name); } \
  NS_IMETHOD MoveToNative(nsIFile *parent_dir, const nsACString & name) { return !_to ? NS_ERROR_NULL_POINTER : _to->MoveToNative(parent_dir, name); } \
  NS_IMETHOD Remove(PRBool recursive) { return !_to ? NS_ERROR_NULL_POINTER : _to->Remove(recursive); } \
  NS_IMETHOD GetPermissions(PRUint32 *aPermissions) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPermissions(aPermissions); } \
  NS_IMETHOD SetPermissions(PRUint32 aPermissions) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPermissions(aPermissions); } \
  NS_IMETHOD GetPermissionsOfLink(PRUint32 *aPermissionsOfLink) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPermissionsOfLink(aPermissionsOfLink); } \
  NS_IMETHOD SetPermissionsOfLink(PRUint32 aPermissionsOfLink) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPermissionsOfLink(aPermissionsOfLink); } \
  NS_IMETHOD GetLastModifiedTime(PRInt64 *aLastModifiedTime) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastModifiedTime(aLastModifiedTime); } \
  NS_IMETHOD SetLastModifiedTime(PRInt64 aLastModifiedTime) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLastModifiedTime(aLastModifiedTime); } \
  NS_IMETHOD GetLastModifiedTimeOfLink(PRInt64 *aLastModifiedTimeOfLink) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastModifiedTimeOfLink(aLastModifiedTimeOfLink); } \
  NS_IMETHOD SetLastModifiedTimeOfLink(PRInt64 aLastModifiedTimeOfLink) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLastModifiedTimeOfLink(aLastModifiedTimeOfLink); } \
  NS_IMETHOD GetFileSize(PRInt64 *aFileSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFileSize(aFileSize); } \
  NS_IMETHOD SetFileSize(PRInt64 aFileSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFileSize(aFileSize); } \
  NS_IMETHOD GetFileSizeOfLink(PRInt64 *aFileSizeOfLink) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFileSizeOfLink(aFileSizeOfLink); } \
  NS_IMETHOD GetTarget(nsAString & aTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTarget(aTarget); } \
  NS_IMETHOD GetNativeTarget(nsACString & aNativeTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNativeTarget(aNativeTarget); } \
  NS_IMETHOD GetPath(nsAString & aPath) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPath(aPath); } \
  NS_IMETHOD GetNativePath(nsACString & aNativePath) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNativePath(aNativePath); } \
  NS_IMETHOD Exists(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Exists(_retval); } \
  NS_IMETHOD IsWritable(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsWritable(_retval); } \
  NS_IMETHOD IsReadable(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsReadable(_retval); } \
  NS_IMETHOD IsExecutable(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsExecutable(_retval); } \
  NS_IMETHOD IsHidden(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsHidden(_retval); } \
  NS_IMETHOD IsDirectory(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsDirectory(_retval); } \
  NS_IMETHOD IsFile(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsFile(_retval); } \
  NS_IMETHOD IsSymlink(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsSymlink(_retval); } \
  NS_IMETHOD IsSpecial(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsSpecial(_retval); } \
  NS_IMETHOD CreateUnique(PRUint32 type, PRUint32 permissions) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateUnique(type, permissions); } \
  NS_IMETHOD Clone(nsIFile **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Clone(_retval); } \
  NS_IMETHOD Equals(nsIFile *file, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Equals(file, _retval); } \
  NS_IMETHOD Contains(nsIFile *file, PRBool recurse, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Contains(file, recurse, _retval); } \
  NS_IMETHOD GetParent(nsIFile * *aParent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParent(aParent); } \
  NS_IMETHOD GetDirectoryEntries(nsISimpleEnumerator * *aDirectoryEntries) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDirectoryEntries(aDirectoryEntries); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsFile : public nsIFile
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIFILE

  nsFile();

private:
  ~nsFile();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsFile, nsIFile)

nsFile::nsFile()
{
  /* member initializers and constructor code */
}

nsFile::~nsFile()
{
  /* destructor code */
}

/* void append (in AString node); */
NS_IMETHODIMP nsFile::Append(const nsAString & node)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void appendNative (in ACString node); */
NS_IMETHODIMP nsFile::AppendNative(const nsACString & node)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void normalize (); */
NS_IMETHODIMP nsFile::Normalize()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void create (in PRUint32 type, in PRUint32 permissions); */
NS_IMETHODIMP nsFile::Create(PRUint32 type, PRUint32 permissions)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AString leafName; */
NS_IMETHODIMP nsFile::GetLeafName(nsAString & aLeafName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFile::SetLeafName(const nsAString & aLeafName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] attribute ACString nativeLeafName; */
NS_IMETHODIMP nsFile::GetNativeLeafName(nsACString & aNativeLeafName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFile::SetNativeLeafName(const nsACString & aNativeLeafName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void copyTo (in nsIFile parent_dir, in AString name); */
NS_IMETHODIMP nsFile::CopyTo(nsIFile *parent_dir, const nsAString & name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void copyToNative (in nsIFile parent_dir, in ACString name); */
NS_IMETHODIMP nsFile::CopyToNative(nsIFile *parent_dir, const nsACString & name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void copyToFollowingLinks (in nsIFile parent_dir, in AString name); */
NS_IMETHODIMP nsFile::CopyToFollowingLinks(nsIFile *parent_dir, const nsAString & name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void copyToFollowingLinksNative (in nsIFile parent_dir, in ACString name); */
NS_IMETHODIMP nsFile::CopyToFollowingLinksNative(nsIFile *parent_dir, const nsACString & name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void moveTo (in nsIFile parent_dir, in AString name); */
NS_IMETHODIMP nsFile::MoveTo(nsIFile *parent_dir, const nsAString & name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void moveToNative (in nsIFile parent_dir, in ACString name); */
NS_IMETHODIMP nsFile::MoveToNative(nsIFile *parent_dir, const nsACString & name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void remove (in boolean recursive); */
NS_IMETHODIMP nsFile::Remove(PRBool recursive)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute unsigned long permissions; */
NS_IMETHODIMP nsFile::GetPermissions(PRUint32 *aPermissions)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFile::SetPermissions(PRUint32 aPermissions)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute unsigned long permissionsOfLink; */
NS_IMETHODIMP nsFile::GetPermissionsOfLink(PRUint32 *aPermissionsOfLink)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFile::SetPermissionsOfLink(PRUint32 aPermissionsOfLink)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute PRInt64 lastModifiedTime; */
NS_IMETHODIMP nsFile::GetLastModifiedTime(PRInt64 *aLastModifiedTime)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFile::SetLastModifiedTime(PRInt64 aLastModifiedTime)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute PRInt64 lastModifiedTimeOfLink; */
NS_IMETHODIMP nsFile::GetLastModifiedTimeOfLink(PRInt64 *aLastModifiedTimeOfLink)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFile::SetLastModifiedTimeOfLink(PRInt64 aLastModifiedTimeOfLink)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute PRInt64 fileSize; */
NS_IMETHODIMP nsFile::GetFileSize(PRInt64 *aFileSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsFile::SetFileSize(PRInt64 aFileSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute PRInt64 fileSizeOfLink; */
NS_IMETHODIMP nsFile::GetFileSizeOfLink(PRInt64 *aFileSizeOfLink)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString target; */
NS_IMETHODIMP nsFile::GetTarget(nsAString & aTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] readonly attribute ACString nativeTarget; */
NS_IMETHODIMP nsFile::GetNativeTarget(nsACString & aNativeTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString path; */
NS_IMETHODIMP nsFile::GetPath(nsAString & aPath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] readonly attribute ACString nativePath; */
NS_IMETHODIMP nsFile::GetNativePath(nsACString & aNativePath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean exists (); */
NS_IMETHODIMP nsFile::Exists(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isWritable (); */
NS_IMETHODIMP nsFile::IsWritable(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isReadable (); */
NS_IMETHODIMP nsFile::IsReadable(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isExecutable (); */
NS_IMETHODIMP nsFile::IsExecutable(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isHidden (); */
NS_IMETHODIMP nsFile::IsHidden(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isDirectory (); */
NS_IMETHODIMP nsFile::IsDirectory(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isFile (); */
NS_IMETHODIMP nsFile::IsFile(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isSymlink (); */
NS_IMETHODIMP nsFile::IsSymlink(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isSpecial (); */
NS_IMETHODIMP nsFile::IsSpecial(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void createUnique (in PRUint32 type, in PRUint32 permissions); */
NS_IMETHODIMP nsFile::CreateUnique(PRUint32 type, PRUint32 permissions)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIFile clone (); */
NS_IMETHODIMP nsFile::Clone(nsIFile **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean equals (in nsIFile file); */
NS_IMETHODIMP nsFile::Equals(nsIFile *file, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean contains (in nsIFile file, in boolean recurse); */
NS_IMETHODIMP nsFile::Contains(nsIFile *file, PRBool recurse, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIFile parent; */
NS_IMETHODIMP nsFile::GetParent(nsIFile * *aParent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsISimpleEnumerator directoryEntries; */
NS_IMETHODIMP nsFile::GetDirectoryEntries(nsISimpleEnumerator * *aDirectoryEntries)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

struct PRFileDesc;
struct PRLibrary;

/* starting interface:    nsILocalFile */
#define NS_ILOCALFILE_IID_STR "aa610f20-a889-11d3-8c81-000064657374"

#define NS_ILOCALFILE_IID \
  {0xaa610f20, 0xa889, 0x11d3, \
    { 0x8c, 0x81, 0x00, 0x00, 0x64, 0x65, 0x73, 0x74 }}

class NS_NO_VTABLE nsILocalFile : public nsIFile {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ILOCALFILE_IID)

  /* void initWithPath (in AString file_path); */
  NS_IMETHOD InitWithPath(const nsAString & file_path) = 0;

  /* [noscript] void initWithNativePath (in ACString file_path); */
  NS_IMETHOD InitWithNativePath(const nsACString & file_path) = 0;

  /* void initWithFile (in nsILocalFile file); */
  NS_IMETHOD InitWithFile(nsILocalFile *file) = 0;

  /* attribute boolean followLinks; */
  NS_IMETHOD GetFollowLinks(PRBool *aFollowLinks) = 0;
  NS_IMETHOD SetFollowLinks(PRBool aFollowLinks) = 0;

  /* [noscript] PRFileDescStar openNSPRFileDesc (in long flags, in long mode); */
  NS_IMETHOD OpenNSPRFileDesc(PRInt32 flags, PRInt32 mode, PRFileDesc * *_retval) = 0;

  /* [noscript] FILEStar openANSIFileDesc (in string mode); */
  NS_IMETHOD OpenANSIFileDesc(const char *mode, FILE * *_retval) = 0;

  /* [noscript] PRLibraryStar load (); */
  NS_IMETHOD Load(PRLibrary * *_retval) = 0;

  /* void appendRelativePath (in AString rel_path); */
  NS_IMETHOD AppendRelativePath(const nsAString & rel_path) = 0;

  /* [noscript] void appendRelativeNativePath (in ACString rel_path); */
  NS_IMETHOD AppendRelativeNativePath(const nsACString & rel_path) = 0;

  /* attribute ACString persistentDescriptor; */
  NS_IMETHOD GetPersistentDescriptor(nsACString & aPersistentDescriptor) = 0;
  NS_IMETHOD SetPersistentDescriptor(const nsACString & aPersistentDescriptor) = 0;

  /* void reveal (); */
  NS_IMETHOD Reveal(void) = 0;

  /* void launch (); */
  NS_IMETHOD Launch(void) = 0;

  /* ACString getRelativeDescriptor (in nsILocalFile from_file); */
  NS_IMETHOD GetRelativeDescriptor(nsILocalFile *from_file, nsACString & _retval) = 0;

  /* void setRelativeDescriptor (in nsILocalFile from_file, in ACString rel_desc); */
  NS_IMETHOD SetRelativeDescriptor(nsILocalFile *from_file, const nsACString & rel_desc) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSILOCALFILE \
  NS_IMETHOD InitWithPath(const nsAString & file_path); \
  NS_IMETHOD InitWithNativePath(const nsACString & file_path); \
  NS_IMETHOD InitWithFile(nsILocalFile *file); \
  NS_IMETHOD GetFollowLinks(PRBool *aFollowLinks); \
  NS_IMETHOD SetFollowLinks(PRBool aFollowLinks); \
  NS_IMETHOD OpenNSPRFileDesc(PRInt32 flags, PRInt32 mode, PRFileDesc * *_retval); \
  NS_IMETHOD OpenANSIFileDesc(const char *mode, FILE * *_retval); \
  NS_IMETHOD Load(PRLibrary * *_retval); \
  NS_IMETHOD AppendRelativePath(const nsAString & rel_path); \
  NS_IMETHOD AppendRelativeNativePath(const nsACString & rel_path); \
  NS_IMETHOD GetPersistentDescriptor(nsACString & aPersistentDescriptor); \
  NS_IMETHOD SetPersistentDescriptor(const nsACString & aPersistentDescriptor); \
  NS_IMETHOD Reveal(void); \
  NS_IMETHOD Launch(void); \
  NS_IMETHOD GetRelativeDescriptor(nsILocalFile *from_file, nsACString & _retval); \
  NS_IMETHOD SetRelativeDescriptor(nsILocalFile *from_file, const nsACString & rel_desc); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSILOCALFILE(_to) \
  NS_IMETHOD InitWithPath(const nsAString & file_path) { return _to InitWithPath(file_path); } \
  NS_IMETHOD InitWithNativePath(const nsACString & file_path) { return _to InitWithNativePath(file_path); } \
  NS_IMETHOD InitWithFile(nsILocalFile *file) { return _to InitWithFile(file); } \
  NS_IMETHOD GetFollowLinks(PRBool *aFollowLinks) { return _to GetFollowLinks(aFollowLinks); } \
  NS_IMETHOD SetFollowLinks(PRBool aFollowLinks) { return _to SetFollowLinks(aFollowLinks); } \
  NS_IMETHOD OpenNSPRFileDesc(PRInt32 flags, PRInt32 mode, PRFileDesc * *_retval) { return _to OpenNSPRFileDesc(flags, mode, _retval); } \
  NS_IMETHOD OpenANSIFileDesc(const char *mode, FILE * *_retval) { return _to OpenANSIFileDesc(mode, _retval); } \
  NS_IMETHOD Load(PRLibrary * *_retval) { return _to Load(_retval); } \
  NS_IMETHOD AppendRelativePath(const nsAString & rel_path) { return _to AppendRelativePath(rel_path); } \
  NS_IMETHOD AppendRelativeNativePath(const nsACString & rel_path) { return _to AppendRelativeNativePath(rel_path); } \
  NS_IMETHOD GetPersistentDescriptor(nsACString & aPersistentDescriptor) { return _to GetPersistentDescriptor(aPersistentDescriptor); } \
  NS_IMETHOD SetPersistentDescriptor(const nsACString & aPersistentDescriptor) { return _to SetPersistentDescriptor(aPersistentDescriptor); } \
  NS_IMETHOD Reveal(void) { return _to Reveal(); } \
  NS_IMETHOD Launch(void) { return _to Launch(); } \
  NS_IMETHOD GetRelativeDescriptor(nsILocalFile *from_file, nsACString & _retval) { return _to GetRelativeDescriptor(from_file, _retval); } \
  NS_IMETHOD SetRelativeDescriptor(nsILocalFile *from_file, const nsACString & rel_desc) { return _to SetRelativeDescriptor(from_file, rel_desc); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSILOCALFILE(_to) \
  NS_IMETHOD InitWithPath(const nsAString & file_path) { return !_to ? NS_ERROR_NULL_POINTER : _to->InitWithPath(file_path); } \
  NS_IMETHOD InitWithNativePath(const nsACString & file_path) { return !_to ? NS_ERROR_NULL_POINTER : _to->InitWithNativePath(file_path); } \
  NS_IMETHOD InitWithFile(nsILocalFile *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->InitWithFile(file); } \
  NS_IMETHOD GetFollowLinks(PRBool *aFollowLinks) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFollowLinks(aFollowLinks); } \
  NS_IMETHOD SetFollowLinks(PRBool aFollowLinks) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFollowLinks(aFollowLinks); } \
  NS_IMETHOD OpenNSPRFileDesc(PRInt32 flags, PRInt32 mode, PRFileDesc * *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenNSPRFileDesc(flags, mode, _retval); } \
  NS_IMETHOD OpenANSIFileDesc(const char *mode, FILE * *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenANSIFileDesc(mode, _retval); } \
  NS_IMETHOD Load(PRLibrary * *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Load(_retval); } \
  NS_IMETHOD AppendRelativePath(const nsAString & rel_path) { return !_to ? NS_ERROR_NULL_POINTER : _to->AppendRelativePath(rel_path); } \
  NS_IMETHOD AppendRelativeNativePath(const nsACString & rel_path) { return !_to ? NS_ERROR_NULL_POINTER : _to->AppendRelativeNativePath(rel_path); } \
  NS_IMETHOD GetPersistentDescriptor(nsACString & aPersistentDescriptor) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPersistentDescriptor(aPersistentDescriptor); } \
  NS_IMETHOD SetPersistentDescriptor(const nsACString & aPersistentDescriptor) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPersistentDescriptor(aPersistentDescriptor); } \
  NS_IMETHOD Reveal(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Reveal(); } \
  NS_IMETHOD Launch(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Launch(); } \
  NS_IMETHOD GetRelativeDescriptor(nsILocalFile *from_file, nsACString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRelativeDescriptor(from_file, _retval); } \
  NS_IMETHOD SetRelativeDescriptor(nsILocalFile *from_file, const nsACString & rel_desc) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetRelativeDescriptor(from_file, rel_desc); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsLocalFile : public nsILocalFile
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSILOCALFILE

  nsLocalFile();

private:
  ~nsLocalFile();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsLocalFile, nsILocalFile)

nsLocalFile::nsLocalFile()
{
  /* member initializers and constructor code */
}

nsLocalFile::~nsLocalFile()
{
  /* destructor code */
}

/* void initWithPath (in AString file_path); */
NS_IMETHODIMP nsLocalFile::InitWithPath(const nsAString & file_path)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void initWithNativePath (in ACString file_path); */
NS_IMETHODIMP nsLocalFile::InitWithNativePath(const nsACString & file_path)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void initWithFile (in nsILocalFile file); */
NS_IMETHODIMP nsLocalFile::InitWithFile(nsILocalFile *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean followLinks; */
NS_IMETHODIMP nsLocalFile::GetFollowLinks(PRBool *aFollowLinks)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsLocalFile::SetFollowLinks(PRBool aFollowLinks)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] PRFileDescStar openNSPRFileDesc (in long flags, in long mode); */
NS_IMETHODIMP nsLocalFile::OpenNSPRFileDesc(PRInt32 flags, PRInt32 mode, PRFileDesc * *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] FILEStar openANSIFileDesc (in string mode); */
NS_IMETHODIMP nsLocalFile::OpenANSIFileDesc(const char *mode, FILE * *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] PRLibraryStar load (); */
NS_IMETHODIMP nsLocalFile::Load(PRLibrary * *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void appendRelativePath (in AString rel_path); */
NS_IMETHODIMP nsLocalFile::AppendRelativePath(const nsAString & rel_path)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void appendRelativeNativePath (in ACString rel_path); */
NS_IMETHODIMP nsLocalFile::AppendRelativeNativePath(const nsACString & rel_path)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute ACString persistentDescriptor; */
NS_IMETHODIMP nsLocalFile::GetPersistentDescriptor(nsACString & aPersistentDescriptor)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsLocalFile::SetPersistentDescriptor(const nsACString & aPersistentDescriptor)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void reveal (); */
NS_IMETHODIMP nsLocalFile::Reveal()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void launch (); */
NS_IMETHODIMP nsLocalFile::Launch()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* ACString getRelativeDescriptor (in nsILocalFile from_file); */
NS_IMETHODIMP nsLocalFile::GetRelativeDescriptor(nsILocalFile *from_file, nsACString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setRelativeDescriptor (in nsILocalFile from_file, in ACString rel_desc); */
NS_IMETHODIMP nsLocalFile::SetRelativeDescriptor(nsILocalFile *from_file, const nsACString & rel_desc)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIEventQueue; /* forward declaration */


/* starting interface:    nsIAppShell */
#define NS_IAPPSHELL_IID_STR "a0757c31-eeac-11d1-9ec1-00aa002fb821"

#define NS_IAPPSHELL_IID \
  {0xa0757c31, 0xeeac, 0x11d1, \
    { 0x9e, 0xc1, 0x00, 0xaa, 0x00, 0x2f, 0xb8, 0x21 }}

class NS_NO_VTABLE nsIAppShell : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IAPPSHELL_IID)

  /* void Create (inout int argc, inout string argv); */
  NS_IMETHOD Create(int *argc, char **argv) = 0;

  /* void Run (); */
  NS_IMETHOD Run(void) = 0;

  /* void Spinup (); */
  NS_IMETHOD Spinup(void) = 0;

  /* void Spindown (); */
  NS_IMETHOD Spindown(void) = 0;

  /* void ListenToEventQueue (in nsIEventQueue queue, in boolean listen); */
  NS_IMETHOD ListenToEventQueue(nsIEventQueue *queue, PRBool listen) = 0;

  /* void GetNativeEvent (in PRBoolRef realEvent, in voidPtrRef event); */
  NS_IMETHOD GetNativeEvent(PRBool & realEvent, void* & event) = 0;

  /* void DispatchNativeEvent (in boolean realEvent, in voidPtr event); */
  NS_IMETHOD DispatchNativeEvent(PRBool realEvent, void * event) = 0;

  /* void Exit (); */
  NS_IMETHOD Exit(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIAPPSHELL \
  NS_IMETHOD Create(int *argc, char **argv); \
  NS_IMETHOD Run(void); \
  NS_IMETHOD Spinup(void); \
  NS_IMETHOD Spindown(void); \
  NS_IMETHOD ListenToEventQueue(nsIEventQueue *queue, PRBool listen); \
  NS_IMETHOD GetNativeEvent(PRBool & realEvent, void* & event); \
  NS_IMETHOD DispatchNativeEvent(PRBool realEvent, void * event); \
  NS_IMETHOD Exit(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIAPPSHELL(_to) \
  NS_IMETHOD Create(int *argc, char **argv) { return _to Create(argc, argv); } \
  NS_IMETHOD Run(void) { return _to Run(); } \
  NS_IMETHOD Spinup(void) { return _to Spinup(); } \
  NS_IMETHOD Spindown(void) { return _to Spindown(); } \
  NS_IMETHOD ListenToEventQueue(nsIEventQueue *queue, PRBool listen) { return _to ListenToEventQueue(queue, listen); } \
  NS_IMETHOD GetNativeEvent(PRBool & realEvent, void* & event) { return _to GetNativeEvent(realEvent, event); } \
  NS_IMETHOD DispatchNativeEvent(PRBool realEvent, void * event) { return _to DispatchNativeEvent(realEvent, event); } \
  NS_IMETHOD Exit(void) { return _to Exit(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIAPPSHELL(_to) \
  NS_IMETHOD Create(int *argc, char **argv) { return !_to ? NS_ERROR_NULL_POINTER : _to->Create(argc, argv); } \
  NS_IMETHOD Run(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Run(); } \
  NS_IMETHOD Spinup(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Spinup(); } \
  NS_IMETHOD Spindown(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Spindown(); } \
  NS_IMETHOD ListenToEventQueue(nsIEventQueue *queue, PRBool listen) { return !_to ? NS_ERROR_NULL_POINTER : _to->ListenToEventQueue(queue, listen); } \
  NS_IMETHOD GetNativeEvent(PRBool & realEvent, void* & event) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNativeEvent(realEvent, event); } \
  NS_IMETHOD DispatchNativeEvent(PRBool realEvent, void * event) { return !_to ? NS_ERROR_NULL_POINTER : _to->DispatchNativeEvent(realEvent, event); } \
  NS_IMETHOD Exit(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Exit(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsAppShell : public nsIAppShell
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIAPPSHELL

  nsAppShell();

private:
  ~nsAppShell();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsAppShell, nsIAppShell)

nsAppShell::nsAppShell()
{
  /* member initializers and constructor code */
}

nsAppShell::~nsAppShell()
{
  /* destructor code */
}

/* void Create (inout int argc, inout string argv); */
NS_IMETHODIMP nsAppShell::Create(int *argc, char **argv)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Run (); */
NS_IMETHODIMP nsAppShell::Run()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Spinup (); */
NS_IMETHODIMP nsAppShell::Spinup()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Spindown (); */
NS_IMETHODIMP nsAppShell::Spindown()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void ListenToEventQueue (in nsIEventQueue queue, in boolean listen); */
NS_IMETHODIMP nsAppShell::ListenToEventQueue(nsIEventQueue *queue, PRBool listen)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void GetNativeEvent (in PRBoolRef realEvent, in voidPtrRef event); */
NS_IMETHODIMP nsAppShell::GetNativeEvent(PRBool & realEvent, void* & event)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void DispatchNativeEvent (in boolean realEvent, in voidPtr event); */
NS_IMETHODIMP nsAppShell::DispatchNativeEvent(PRBool realEvent, void * event)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Exit (); */
NS_IMETHODIMP nsAppShell::Exit()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIInterfaceRequestor; /* forward declaration */

class nsIX509Cert; /* forward declaration */


/* starting interface:    nsIBadCertListener */
#define NS_IBADCERTLISTENER_IID_STR "86960956-edb0-11d4-998b-00b0d02354a0"

#define NS_IBADCERTLISTENER_IID \
  {0x86960956, 0xedb0, 0x11d4, \
    { 0x99, 0x8b, 0x00, 0xb0, 0xd0, 0x23, 0x54, 0xa0 }}

class NS_NO_VTABLE nsIBadCertListener : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IBADCERTLISTENER_IID)

  enum { UNINIT_ADD_FLAG = -1 };

  enum { ADD_TRUSTED_FOR_SESSION = 1 };

  enum { ADD_TRUSTED_PERMANENTLY = 2 };

  /* boolean confirmUnknownIssuer (in nsIInterfaceRequestor socketInfo, in nsIX509Cert cert, out short certAddType); */
  NS_IMETHOD ConfirmUnknownIssuer(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRInt16 *certAddType, PRBool *_retval) = 0;

  /* boolean confirmMismatchDomain (in nsIInterfaceRequestor socketInfo, in AUTF8String targetURL, in nsIX509Cert cert); */
  NS_IMETHOD ConfirmMismatchDomain(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert, PRBool *_retval) = 0;

  /* boolean confirmCertExpired (in nsIInterfaceRequestor socketInfo, in nsIX509Cert cert); */
  NS_IMETHOD ConfirmCertExpired(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRBool *_retval) = 0;

  /* void notifyCrlNextupdate (in nsIInterfaceRequestor socketInfo, in AUTF8String targetURL, in nsIX509Cert cert); */
  NS_IMETHOD NotifyCrlNextupdate(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIBADCERTLISTENER \
  NS_IMETHOD ConfirmUnknownIssuer(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRInt16 *certAddType, PRBool *_retval); \
  NS_IMETHOD ConfirmMismatchDomain(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert, PRBool *_retval); \
  NS_IMETHOD ConfirmCertExpired(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRBool *_retval); \
  NS_IMETHOD NotifyCrlNextupdate(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIBADCERTLISTENER(_to) \
  NS_IMETHOD ConfirmUnknownIssuer(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRInt16 *certAddType, PRBool *_retval) { return _to ConfirmUnknownIssuer(socketInfo, cert, certAddType, _retval); } \
  NS_IMETHOD ConfirmMismatchDomain(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert, PRBool *_retval) { return _to ConfirmMismatchDomain(socketInfo, targetURL, cert, _retval); } \
  NS_IMETHOD ConfirmCertExpired(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRBool *_retval) { return _to ConfirmCertExpired(socketInfo, cert, _retval); } \
  NS_IMETHOD NotifyCrlNextupdate(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert) { return _to NotifyCrlNextupdate(socketInfo, targetURL, cert); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIBADCERTLISTENER(_to) \
  NS_IMETHOD ConfirmUnknownIssuer(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRInt16 *certAddType, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConfirmUnknownIssuer(socketInfo, cert, certAddType, _retval); } \
  NS_IMETHOD ConfirmMismatchDomain(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConfirmMismatchDomain(socketInfo, targetURL, cert, _retval); } \
  NS_IMETHOD ConfirmCertExpired(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConfirmCertExpired(socketInfo, cert, _retval); } \
  NS_IMETHOD NotifyCrlNextupdate(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert) { return !_to ? NS_ERROR_NULL_POINTER : _to->NotifyCrlNextupdate(socketInfo, targetURL, cert); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsBadCertListener : public nsIBadCertListener
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIBADCERTLISTENER

  nsBadCertListener();

private:
  ~nsBadCertListener();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsBadCertListener, nsIBadCertListener)

nsBadCertListener::nsBadCertListener()
{
  /* member initializers and constructor code */
}

nsBadCertListener::~nsBadCertListener()
{
  /* destructor code */
}

/* boolean confirmUnknownIssuer (in nsIInterfaceRequestor socketInfo, in nsIX509Cert cert, out short certAddType); */
NS_IMETHODIMP nsBadCertListener::ConfirmUnknownIssuer(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRInt16 *certAddType, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean confirmMismatchDomain (in nsIInterfaceRequestor socketInfo, in AUTF8String targetURL, in nsIX509Cert cert); */
NS_IMETHODIMP nsBadCertListener::ConfirmMismatchDomain(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean confirmCertExpired (in nsIInterfaceRequestor socketInfo, in nsIX509Cert cert); */
NS_IMETHODIMP nsBadCertListener::ConfirmCertExpired(nsIInterfaceRequestor *socketInfo, nsIX509Cert *cert, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void notifyCrlNextupdate (in nsIInterfaceRequestor socketInfo, in AUTF8String targetURL, in nsIX509Cert cert); */
NS_IMETHODIMP nsBadCertListener::NotifyCrlNextupdate(nsIInterfaceRequestor *socketInfo, const nsACString & targetURL, nsIX509Cert *cert)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

typedef void * nativeWindow;

    class nsIWidget;

/* starting interface:    nsIBaseWindow */
#define NS_IBASEWINDOW_IID_STR "046bc8a0-8015-11d3-af70-00a024ffc08c"

#define NS_IBASEWINDOW_IID \
  {0x046bc8a0, 0x8015, 0x11d3, \
    { 0xaf, 0x70, 0x00, 0xa0, 0x24, 0xff, 0xc0, 0x8c }}

class NS_NO_VTABLE nsIBaseWindow : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IBASEWINDOW_IID)

  /* [noscript] void initWindow (in nativeWindow parentNativeWindow, in nsIWidget parentWidget, in long x, in long y, in long cx, in long cy); */
  NS_IMETHOD InitWindow(nativeWindow parentNativeWindow, nsIWidget * parentWidget, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy) = 0;

  /* void create (); */
  NS_IMETHOD Create(void) = 0;

  /* void destroy (); */
  NS_IMETHOD Destroy(void) = 0;

  /* void setPosition (in long x, in long y); */
  NS_IMETHOD SetPosition(PRInt32 x, PRInt32 y) = 0;

  /* void getPosition (out long x, out long y); */
  NS_IMETHOD GetPosition(PRInt32 *x, PRInt32 *y) = 0;

  /* void setSize (in long cx, in long cy, in boolean repaint); */
  NS_IMETHOD SetSize(PRInt32 cx, PRInt32 cy, PRBool repaint) = 0;

  /* void getSize (out long cx, out long cy); */
  NS_IMETHOD GetSize(PRInt32 *cx, PRInt32 *cy) = 0;

  /* void setPositionAndSize (in long x, in long y, in long cx, in long cy, in boolean repaint); */
  NS_IMETHOD SetPositionAndSize(PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy, PRBool repaint) = 0;

  /* void getPositionAndSize (out long x, out long y, out long cx, out long cy); */
  NS_IMETHOD GetPositionAndSize(PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy) = 0;

  /* void repaint (in boolean force); */
  NS_IMETHOD Repaint(PRBool force) = 0;

  /* [noscript] attribute nsIWidget parentWidget; */
  NS_IMETHOD GetParentWidget(nsIWidget * *aParentWidget) = 0;
  NS_IMETHOD SetParentWidget(nsIWidget * aParentWidget) = 0;

  /* attribute nativeWindow parentNativeWindow; */
  NS_IMETHOD GetParentNativeWindow(nativeWindow *aParentNativeWindow) = 0;
  NS_IMETHOD SetParentNativeWindow(nativeWindow aParentNativeWindow) = 0;

  /* attribute boolean visibility; */
  NS_IMETHOD GetVisibility(PRBool *aVisibility) = 0;
  NS_IMETHOD SetVisibility(PRBool aVisibility) = 0;

  /* attribute boolean enabled; */
  NS_IMETHOD GetEnabled(PRBool *aEnabled) = 0;
  NS_IMETHOD SetEnabled(PRBool aEnabled) = 0;

  /* attribute boolean blurSuppression; */
  NS_IMETHOD GetBlurSuppression(PRBool *aBlurSuppression) = 0;
  NS_IMETHOD SetBlurSuppression(PRBool aBlurSuppression) = 0;

  /* [noscript] readonly attribute nsIWidget mainWidget; */
  NS_IMETHOD GetMainWidget(nsIWidget * *aMainWidget) = 0;

  /* void setFocus (); */
  NS_IMETHOD SetFocus(void) = 0;

  /* attribute wstring title; */
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) = 0;
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIBASEWINDOW \
  NS_IMETHOD InitWindow(nativeWindow parentNativeWindow, nsIWidget * parentWidget, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy); \
  NS_IMETHOD Create(void); \
  NS_IMETHOD Destroy(void); \
  NS_IMETHOD SetPosition(PRInt32 x, PRInt32 y); \
  NS_IMETHOD GetPosition(PRInt32 *x, PRInt32 *y); \
  NS_IMETHOD SetSize(PRInt32 cx, PRInt32 cy, PRBool repaint); \
  NS_IMETHOD GetSize(PRInt32 *cx, PRInt32 *cy); \
  NS_IMETHOD SetPositionAndSize(PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy, PRBool repaint); \
  NS_IMETHOD GetPositionAndSize(PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy); \
  NS_IMETHOD Repaint(PRBool force); \
  NS_IMETHOD GetParentWidget(nsIWidget * *aParentWidget); \
  NS_IMETHOD SetParentWidget(nsIWidget * aParentWidget); \
  NS_IMETHOD GetParentNativeWindow(nativeWindow *aParentNativeWindow); \
  NS_IMETHOD SetParentNativeWindow(nativeWindow aParentNativeWindow); \
  NS_IMETHOD GetVisibility(PRBool *aVisibility); \
  NS_IMETHOD SetVisibility(PRBool aVisibility); \
  NS_IMETHOD GetEnabled(PRBool *aEnabled); \
  NS_IMETHOD SetEnabled(PRBool aEnabled); \
  NS_IMETHOD GetBlurSuppression(PRBool *aBlurSuppression); \
  NS_IMETHOD SetBlurSuppression(PRBool aBlurSuppression); \
  NS_IMETHOD GetMainWidget(nsIWidget * *aMainWidget); \
  NS_IMETHOD SetFocus(void); \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle); \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIBASEWINDOW(_to) \
  NS_IMETHOD InitWindow(nativeWindow parentNativeWindow, nsIWidget * parentWidget, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy) { return _to InitWindow(parentNativeWindow, parentWidget, x, y, cx, cy); } \
  NS_IMETHOD Create(void) { return _to Create(); } \
  NS_IMETHOD Destroy(void) { return _to Destroy(); } \
  NS_IMETHOD SetPosition(PRInt32 x, PRInt32 y) { return _to SetPosition(x, y); } \
  NS_IMETHOD GetPosition(PRInt32 *x, PRInt32 *y) { return _to GetPosition(x, y); } \
  NS_IMETHOD SetSize(PRInt32 cx, PRInt32 cy, PRBool repaint) { return _to SetSize(cx, cy, repaint); } \
  NS_IMETHOD GetSize(PRInt32 *cx, PRInt32 *cy) { return _to GetSize(cx, cy); } \
  NS_IMETHOD SetPositionAndSize(PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy, PRBool repaint) { return _to SetPositionAndSize(x, y, cx, cy, repaint); } \
  NS_IMETHOD GetPositionAndSize(PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy) { return _to GetPositionAndSize(x, y, cx, cy); } \
  NS_IMETHOD Repaint(PRBool force) { return _to Repaint(force); } \
  NS_IMETHOD GetParentWidget(nsIWidget * *aParentWidget) { return _to GetParentWidget(aParentWidget); } \
  NS_IMETHOD SetParentWidget(nsIWidget * aParentWidget) { return _to SetParentWidget(aParentWidget); } \
  NS_IMETHOD GetParentNativeWindow(nativeWindow *aParentNativeWindow) { return _to GetParentNativeWindow(aParentNativeWindow); } \
  NS_IMETHOD SetParentNativeWindow(nativeWindow aParentNativeWindow) { return _to SetParentNativeWindow(aParentNativeWindow); } \
  NS_IMETHOD GetVisibility(PRBool *aVisibility) { return _to GetVisibility(aVisibility); } \
  NS_IMETHOD SetVisibility(PRBool aVisibility) { return _to SetVisibility(aVisibility); } \
  NS_IMETHOD GetEnabled(PRBool *aEnabled) { return _to GetEnabled(aEnabled); } \
  NS_IMETHOD SetEnabled(PRBool aEnabled) { return _to SetEnabled(aEnabled); } \
  NS_IMETHOD GetBlurSuppression(PRBool *aBlurSuppression) { return _to GetBlurSuppression(aBlurSuppression); } \
  NS_IMETHOD SetBlurSuppression(PRBool aBlurSuppression) { return _to SetBlurSuppression(aBlurSuppression); } \
  NS_IMETHOD GetMainWidget(nsIWidget * *aMainWidget) { return _to GetMainWidget(aMainWidget); } \
  NS_IMETHOD SetFocus(void) { return _to SetFocus(); } \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) { return _to GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) { return _to SetTitle(aTitle); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIBASEWINDOW(_to) \
  NS_IMETHOD InitWindow(nativeWindow parentNativeWindow, nsIWidget * parentWidget, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy) { return !_to ? NS_ERROR_NULL_POINTER : _to->InitWindow(parentNativeWindow, parentWidget, x, y, cx, cy); } \
  NS_IMETHOD Create(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Create(); } \
  NS_IMETHOD Destroy(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Destroy(); } \
  NS_IMETHOD SetPosition(PRInt32 x, PRInt32 y) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPosition(x, y); } \
  NS_IMETHOD GetPosition(PRInt32 *x, PRInt32 *y) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPosition(x, y); } \
  NS_IMETHOD SetSize(PRInt32 cx, PRInt32 cy, PRBool repaint) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSize(cx, cy, repaint); } \
  NS_IMETHOD GetSize(PRInt32 *cx, PRInt32 *cy) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSize(cx, cy); } \
  NS_IMETHOD SetPositionAndSize(PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy, PRBool repaint) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPositionAndSize(x, y, cx, cy, repaint); } \
  NS_IMETHOD GetPositionAndSize(PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPositionAndSize(x, y, cx, cy); } \
  NS_IMETHOD Repaint(PRBool force) { return !_to ? NS_ERROR_NULL_POINTER : _to->Repaint(force); } \
  NS_IMETHOD GetParentWidget(nsIWidget * *aParentWidget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParentWidget(aParentWidget); } \
  NS_IMETHOD SetParentWidget(nsIWidget * aParentWidget) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetParentWidget(aParentWidget); } \
  NS_IMETHOD GetParentNativeWindow(nativeWindow *aParentNativeWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParentNativeWindow(aParentNativeWindow); } \
  NS_IMETHOD SetParentNativeWindow(nativeWindow aParentNativeWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetParentNativeWindow(aParentNativeWindow); } \
  NS_IMETHOD GetVisibility(PRBool *aVisibility) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetVisibility(aVisibility); } \
  NS_IMETHOD SetVisibility(PRBool aVisibility) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetVisibility(aVisibility); } \
  NS_IMETHOD GetEnabled(PRBool *aEnabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetEnabled(aEnabled); } \
  NS_IMETHOD SetEnabled(PRBool aEnabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetEnabled(aEnabled); } \
  NS_IMETHOD GetBlurSuppression(PRBool *aBlurSuppression) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBlurSuppression(aBlurSuppression); } \
  NS_IMETHOD SetBlurSuppression(PRBool aBlurSuppression) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetBlurSuppression(aBlurSuppression); } \
  NS_IMETHOD GetMainWidget(nsIWidget * *aMainWidget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMainWidget(aMainWidget); } \
  NS_IMETHOD SetFocus(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFocus(); } \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTitle(aTitle); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsBaseWindow : public nsIBaseWindow
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIBASEWINDOW

  nsBaseWindow();

private:
  ~nsBaseWindow();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsBaseWindow, nsIBaseWindow)

nsBaseWindow::nsBaseWindow()
{
  /* member initializers and constructor code */
}

nsBaseWindow::~nsBaseWindow()
{
  /* destructor code */
}

/* [noscript] void initWindow (in nativeWindow parentNativeWindow, in nsIWidget parentWidget, in long x, in long y, in long cx, in long cy); */
NS_IMETHODIMP nsBaseWindow::InitWindow(nativeWindow parentNativeWindow, nsIWidget * parentWidget, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void create (); */
NS_IMETHODIMP nsBaseWindow::Create()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void destroy (); */
NS_IMETHODIMP nsBaseWindow::Destroy()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setPosition (in long x, in long y); */
NS_IMETHODIMP nsBaseWindow::SetPosition(PRInt32 x, PRInt32 y)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getPosition (out long x, out long y); */
NS_IMETHODIMP nsBaseWindow::GetPosition(PRInt32 *x, PRInt32 *y)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setSize (in long cx, in long cy, in boolean repaint); */
NS_IMETHODIMP nsBaseWindow::SetSize(PRInt32 cx, PRInt32 cy, PRBool repaint)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getSize (out long cx, out long cy); */
NS_IMETHODIMP nsBaseWindow::GetSize(PRInt32 *cx, PRInt32 *cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setPositionAndSize (in long x, in long y, in long cx, in long cy, in boolean repaint); */
NS_IMETHODIMP nsBaseWindow::SetPositionAndSize(PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy, PRBool repaint)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getPositionAndSize (out long x, out long y, out long cx, out long cy); */
NS_IMETHODIMP nsBaseWindow::GetPositionAndSize(PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void repaint (in boolean force); */
NS_IMETHODIMP nsBaseWindow::Repaint(PRBool force)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] attribute nsIWidget parentWidget; */
NS_IMETHODIMP nsBaseWindow::GetParentWidget(nsIWidget * *aParentWidget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsBaseWindow::SetParentWidget(nsIWidget * aParentWidget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nativeWindow parentNativeWindow; */
NS_IMETHODIMP nsBaseWindow::GetParentNativeWindow(nativeWindow *aParentNativeWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsBaseWindow::SetParentNativeWindow(nativeWindow aParentNativeWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean visibility; */
NS_IMETHODIMP nsBaseWindow::GetVisibility(PRBool *aVisibility)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsBaseWindow::SetVisibility(PRBool aVisibility)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean enabled; */
NS_IMETHODIMP nsBaseWindow::GetEnabled(PRBool *aEnabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsBaseWindow::SetEnabled(PRBool aEnabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean blurSuppression; */
NS_IMETHODIMP nsBaseWindow::GetBlurSuppression(PRBool *aBlurSuppression)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsBaseWindow::SetBlurSuppression(PRBool aBlurSuppression)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] readonly attribute nsIWidget mainWidget; */
NS_IMETHODIMP nsBaseWindow::GetMainWidget(nsIWidget * *aMainWidget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setFocus (); */
NS_IMETHODIMP nsBaseWindow::SetFocus()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring title; */
NS_IMETHODIMP nsBaseWindow::GetTitle(PRUnichar * *aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsBaseWindow::SetTitle(const PRUnichar * aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

typedef PRInt32 nsCacheStoragePolicy;

class nsICacheSession; /* forward declaration */

class nsICacheVisitor; /* forward declaration */


/* starting interface:    nsICacheService */
#define NS_ICACHESERVICE_IID_STR "de114eb4-29fc-4959-b2f7-2d03eb9bc771"

#define NS_ICACHESERVICE_IID \
  {0xde114eb4, 0x29fc, 0x4959, \
    { 0xb2, 0xf7, 0x2d, 0x03, 0xeb, 0x9b, 0xc7, 0x71 }}

class NS_NO_VTABLE nsICacheService : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICACHESERVICE_IID)

  /* nsICacheSession createSession (in string clientID, in nsCacheStoragePolicy storagePolicy, in boolean streamBased); */
  NS_IMETHOD CreateSession(const char *clientID, nsCacheStoragePolicy storagePolicy, PRBool streamBased, nsICacheSession **_retval) = 0;

  /* void visitEntries (in nsICacheVisitor visitor); */
  NS_IMETHOD VisitEntries(nsICacheVisitor *visitor) = 0;

  /* void evictEntries (in nsCacheStoragePolicy storagePolicy); */
  NS_IMETHOD EvictEntries(nsCacheStoragePolicy storagePolicy) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICACHESERVICE \
  NS_IMETHOD CreateSession(const char *clientID, nsCacheStoragePolicy storagePolicy, PRBool streamBased, nsICacheSession **_retval); \
  NS_IMETHOD VisitEntries(nsICacheVisitor *visitor); \
  NS_IMETHOD EvictEntries(nsCacheStoragePolicy storagePolicy); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICACHESERVICE(_to) \
  NS_IMETHOD CreateSession(const char *clientID, nsCacheStoragePolicy storagePolicy, PRBool streamBased, nsICacheSession **_retval) { return _to CreateSession(clientID, storagePolicy, streamBased, _retval); } \
  NS_IMETHOD VisitEntries(nsICacheVisitor *visitor) { return _to VisitEntries(visitor); } \
  NS_IMETHOD EvictEntries(nsCacheStoragePolicy storagePolicy) { return _to EvictEntries(storagePolicy); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICACHESERVICE(_to) \
  NS_IMETHOD CreateSession(const char *clientID, nsCacheStoragePolicy storagePolicy, PRBool streamBased, nsICacheSession **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateSession(clientID, storagePolicy, streamBased, _retval); } \
  NS_IMETHOD VisitEntries(nsICacheVisitor *visitor) { return !_to ? NS_ERROR_NULL_POINTER : _to->VisitEntries(visitor); } \
  NS_IMETHOD EvictEntries(nsCacheStoragePolicy storagePolicy) { return !_to ? NS_ERROR_NULL_POINTER : _to->EvictEntries(storagePolicy); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsCacheService : public nsICacheService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICACHESERVICE

  nsCacheService();

private:
  ~nsCacheService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsCacheService, nsICacheService)

nsCacheService::nsCacheService()
{
  /* member initializers and constructor code */
}

nsCacheService::~nsCacheService()
{
  /* destructor code */
}

/* nsICacheSession createSession (in string clientID, in nsCacheStoragePolicy storagePolicy, in boolean streamBased); */
NS_IMETHODIMP nsCacheService::CreateSession(const char *clientID, nsCacheStoragePolicy storagePolicy, PRBool streamBased, nsICacheSession **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void visitEntries (in nsICacheVisitor visitor); */
NS_IMETHODIMP nsCacheService::VisitEntries(nsICacheVisitor *visitor)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void evictEntries (in nsCacheStoragePolicy storagePolicy); */
NS_IMETHODIMP nsCacheService::EvictEntries(nsCacheStoragePolicy storagePolicy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsICancelable */
#define NS_ICANCELABLE_IID_STR "d94ac0a0-bb18-46b8-844e-84159064b0bd"

#define NS_ICANCELABLE_IID \
  {0xd94ac0a0, 0xbb18, 0x46b8, \
    { 0x84, 0x4e, 0x84, 0x15, 0x90, 0x64, 0xb0, 0xbd }}

class NS_NO_VTABLE nsICancelable : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICANCELABLE_IID)

  /* void cancel (in nsresult reason); */
  NS_IMETHOD Cancel(nsresult reason) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICANCELABLE \
  NS_IMETHOD Cancel(nsresult reason); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICANCELABLE(_to) \
  NS_IMETHOD Cancel(nsresult reason) { return _to Cancel(reason); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICANCELABLE(_to) \
  NS_IMETHOD Cancel(nsresult reason) { return !_to ? NS_ERROR_NULL_POINTER : _to->Cancel(reason); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsCancelable : public nsICancelable
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICANCELABLE

  nsCancelable();

private:
  ~nsCancelable();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsCancelable, nsICancelable)

nsCancelable::nsCancelable()
{
  /* member initializers and constructor code */
}

nsCancelable::~nsCancelable()
{
  /* destructor code */
}

/* void cancel (in nsresult reason); */
NS_IMETHODIMP nsCancelable::Cancel(nsresult reason)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsILoadGroup; /* forward declaration */

typedef PRUint32 nsLoadFlags;


/* starting interface:    nsIRequest */
#define NS_IREQUEST_IID_STR "ef6bfbd2-fd46-48d8-96b7-9f8f0fd387fe"

#define NS_IREQUEST_IID \
  {0xef6bfbd2, 0xfd46, 0x48d8, \
    { 0x96, 0xb7, 0x9f, 0x8f, 0x0f, 0xd3, 0x87, 0xfe }}

class NS_NO_VTABLE nsIRequest : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IREQUEST_IID)

  enum { LOAD_NORMAL = 0U };

  enum { LOAD_BACKGROUND = 1U };

  enum { INHIBIT_CACHING = 128U };

  enum { INHIBIT_PERSISTENT_CACHING = 256U };

  enum { LOAD_BYPASS_CACHE = 512U };

  enum { LOAD_FROM_CACHE = 1024U };

  enum { VALIDATE_ALWAYS = 2048U };

  enum { VALIDATE_NEVER = 4096U };

  enum { VALIDATE_ONCE_PER_SESSION = 8192U };

  /* readonly attribute AUTF8String name; */
  NS_IMETHOD GetName(nsACString & aName) = 0;

  /* boolean isPending (); */
  NS_IMETHOD IsPending(PRBool *_retval) = 0;

  /* readonly attribute nsresult status; */
  NS_IMETHOD GetStatus(nsresult *aStatus) = 0;

  /* void cancel (in nsresult status); */
  NS_IMETHOD Cancel(nsresult status) = 0;

  /* void suspend (); */
  NS_IMETHOD Suspend(void) = 0;

  /* void resume (); */
  NS_IMETHOD Resume(void) = 0;

  /* attribute nsILoadGroup loadGroup; */
  NS_IMETHOD GetLoadGroup(nsILoadGroup * *aLoadGroup) = 0;
  NS_IMETHOD SetLoadGroup(nsILoadGroup * aLoadGroup) = 0;

  /* attribute nsLoadFlags loadFlags; */
  NS_IMETHOD GetLoadFlags(nsLoadFlags *aLoadFlags) = 0;
  NS_IMETHOD SetLoadFlags(nsLoadFlags aLoadFlags) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIREQUEST \
  NS_IMETHOD GetName(nsACString & aName); \
  NS_IMETHOD IsPending(PRBool *_retval); \
  NS_IMETHOD GetStatus(nsresult *aStatus); \
  NS_IMETHOD Cancel(nsresult status); \
  NS_IMETHOD Suspend(void); \
  NS_IMETHOD Resume(void); \
  NS_IMETHOD GetLoadGroup(nsILoadGroup * *aLoadGroup); \
  NS_IMETHOD SetLoadGroup(nsILoadGroup * aLoadGroup); \
  NS_IMETHOD GetLoadFlags(nsLoadFlags *aLoadFlags); \
  NS_IMETHOD SetLoadFlags(nsLoadFlags aLoadFlags); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIREQUEST(_to) \
  NS_IMETHOD GetName(nsACString & aName) { return _to GetName(aName); } \
  NS_IMETHOD IsPending(PRBool *_retval) { return _to IsPending(_retval); } \
  NS_IMETHOD GetStatus(nsresult *aStatus) { return _to GetStatus(aStatus); } \
  NS_IMETHOD Cancel(nsresult status) { return _to Cancel(status); } \
  NS_IMETHOD Suspend(void) { return _to Suspend(); } \
  NS_IMETHOD Resume(void) { return _to Resume(); } \
  NS_IMETHOD GetLoadGroup(nsILoadGroup * *aLoadGroup) { return _to GetLoadGroup(aLoadGroup); } \
  NS_IMETHOD SetLoadGroup(nsILoadGroup * aLoadGroup) { return _to SetLoadGroup(aLoadGroup); } \
  NS_IMETHOD GetLoadFlags(nsLoadFlags *aLoadFlags) { return _to GetLoadFlags(aLoadFlags); } \
  NS_IMETHOD SetLoadFlags(nsLoadFlags aLoadFlags) { return _to SetLoadFlags(aLoadFlags); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIREQUEST(_to) \
  NS_IMETHOD GetName(nsACString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD IsPending(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsPending(_retval); } \
  NS_IMETHOD GetStatus(nsresult *aStatus) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetStatus(aStatus); } \
  NS_IMETHOD Cancel(nsresult status) { return !_to ? NS_ERROR_NULL_POINTER : _to->Cancel(status); } \
  NS_IMETHOD Suspend(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Suspend(); } \
  NS_IMETHOD Resume(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Resume(); } \
  NS_IMETHOD GetLoadGroup(nsILoadGroup * *aLoadGroup) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLoadGroup(aLoadGroup); } \
  NS_IMETHOD SetLoadGroup(nsILoadGroup * aLoadGroup) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLoadGroup(aLoadGroup); } \
  NS_IMETHOD GetLoadFlags(nsLoadFlags *aLoadFlags) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLoadFlags(aLoadFlags); } \
  NS_IMETHOD SetLoadFlags(nsLoadFlags aLoadFlags) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLoadFlags(aLoadFlags); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsRequest : public nsIRequest
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIREQUEST

  nsRequest();

private:
  ~nsRequest();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsRequest, nsIRequest)

nsRequest::nsRequest()
{
  /* member initializers and constructor code */
}

nsRequest::~nsRequest()
{
  /* destructor code */
}

/* readonly attribute AUTF8String name; */
NS_IMETHODIMP nsRequest::GetName(nsACString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isPending (); */
NS_IMETHODIMP nsRequest::IsPending(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsresult status; */
NS_IMETHODIMP nsRequest::GetStatus(nsresult *aStatus)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void cancel (in nsresult status); */
NS_IMETHODIMP nsRequest::Cancel(nsresult status)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void suspend (); */
NS_IMETHODIMP nsRequest::Suspend()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void resume (); */
NS_IMETHODIMP nsRequest::Resume()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsILoadGroup loadGroup; */
NS_IMETHODIMP nsRequest::GetLoadGroup(nsILoadGroup * *aLoadGroup)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsRequest::SetLoadGroup(nsILoadGroup * aLoadGroup)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsLoadFlags loadFlags; */
NS_IMETHODIMP nsRequest::GetLoadFlags(nsLoadFlags *aLoadFlags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsRequest::SetLoadFlags(nsLoadFlags aLoadFlags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIURI; /* forward declaration */

class nsIInterfaceRequestor; /* forward declaration */

class nsIInputStream; /* forward declaration */

class nsIStreamListener; /* forward declaration */


/* starting interface:    nsIChannel */
#define NS_ICHANNEL_IID_STR "c63a055a-a676-4e71-bf3c-6cfa11082018"

#define NS_ICHANNEL_IID \
  {0xc63a055a, 0xa676, 0x4e71, \
    { 0xbf, 0x3c, 0x6c, 0xfa, 0x11, 0x08, 0x20, 0x18 }}

class NS_NO_VTABLE nsIChannel : public nsIRequest {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICHANNEL_IID)

  enum { LOAD_DOCUMENT_URI = 65536U };

  enum { LOAD_RETARGETED_DOCUMENT_URI = 131072U };

  enum { LOAD_REPLACE = 262144U };

  enum { LOAD_INITIAL_DOCUMENT_URI = 524288U };

  enum { LOAD_TARGETED = 1048576U };

  /* attribute nsIURI originalURI; */
  NS_IMETHOD GetOriginalURI(nsIURI * *aOriginalURI) = 0;
  NS_IMETHOD SetOriginalURI(nsIURI * aOriginalURI) = 0;

  /* readonly attribute nsIURI URI; */
  NS_IMETHOD GetURI(nsIURI * *aURI) = 0;

  /* attribute nsISupports owner; */
  NS_IMETHOD GetOwner(nsISupports * *aOwner) = 0;
  NS_IMETHOD SetOwner(nsISupports * aOwner) = 0;

  /* attribute nsIInterfaceRequestor notificationCallbacks; */
  NS_IMETHOD GetNotificationCallbacks(nsIInterfaceRequestor * *aNotificationCallbacks) = 0;
  NS_IMETHOD SetNotificationCallbacks(nsIInterfaceRequestor * aNotificationCallbacks) = 0;

  /* readonly attribute nsISupports securityInfo; */
  NS_IMETHOD GetSecurityInfo(nsISupports * *aSecurityInfo) = 0;

  /* attribute ACString contentType; */
  NS_IMETHOD GetContentType(nsACString & aContentType) = 0;
  NS_IMETHOD SetContentType(const nsACString & aContentType) = 0;

  /* attribute ACString contentCharset; */
  NS_IMETHOD GetContentCharset(nsACString & aContentCharset) = 0;
  NS_IMETHOD SetContentCharset(const nsACString & aContentCharset) = 0;

  /* attribute long contentLength; */
  NS_IMETHOD GetContentLength(PRInt32 *aContentLength) = 0;
  NS_IMETHOD SetContentLength(PRInt32 aContentLength) = 0;

  /* nsIInputStream open (); */
  NS_IMETHOD Open(nsIInputStream **_retval) = 0;

  /* void asyncOpen (in nsIStreamListener listener, in nsISupports xcontext); */
  NS_IMETHOD AsyncOpen(nsIStreamListener *listener, nsISupports *xcontext) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICHANNEL \
  NS_IMETHOD GetOriginalURI(nsIURI * *aOriginalURI); \
  NS_IMETHOD SetOriginalURI(nsIURI * aOriginalURI); \
  NS_IMETHOD GetURI(nsIURI * *aURI); \
  NS_IMETHOD GetOwner(nsISupports * *aOwner); \
  NS_IMETHOD SetOwner(nsISupports * aOwner); \
  NS_IMETHOD GetNotificationCallbacks(nsIInterfaceRequestor * *aNotificationCallbacks); \
  NS_IMETHOD SetNotificationCallbacks(nsIInterfaceRequestor * aNotificationCallbacks); \
  NS_IMETHOD GetSecurityInfo(nsISupports * *aSecurityInfo); \
  NS_IMETHOD GetContentType(nsACString & aContentType); \
  NS_IMETHOD SetContentType(const nsACString & aContentType); \
  NS_IMETHOD GetContentCharset(nsACString & aContentCharset); \
  NS_IMETHOD SetContentCharset(const nsACString & aContentCharset); \
  NS_IMETHOD GetContentLength(PRInt32 *aContentLength); \
  NS_IMETHOD SetContentLength(PRInt32 aContentLength); \
  NS_IMETHOD Open(nsIInputStream **_retval); \
  NS_IMETHOD AsyncOpen(nsIStreamListener *listener, nsISupports *xcontext); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICHANNEL(_to) \
  NS_IMETHOD GetOriginalURI(nsIURI * *aOriginalURI) { return _to GetOriginalURI(aOriginalURI); } \
  NS_IMETHOD SetOriginalURI(nsIURI * aOriginalURI) { return _to SetOriginalURI(aOriginalURI); } \
  NS_IMETHOD GetURI(nsIURI * *aURI) { return _to GetURI(aURI); } \
  NS_IMETHOD GetOwner(nsISupports * *aOwner) { return _to GetOwner(aOwner); } \
  NS_IMETHOD SetOwner(nsISupports * aOwner) { return _to SetOwner(aOwner); } \
  NS_IMETHOD GetNotificationCallbacks(nsIInterfaceRequestor * *aNotificationCallbacks) { return _to GetNotificationCallbacks(aNotificationCallbacks); } \
  NS_IMETHOD SetNotificationCallbacks(nsIInterfaceRequestor * aNotificationCallbacks) { return _to SetNotificationCallbacks(aNotificationCallbacks); } \
  NS_IMETHOD GetSecurityInfo(nsISupports * *aSecurityInfo) { return _to GetSecurityInfo(aSecurityInfo); } \
  NS_IMETHOD GetContentType(nsACString & aContentType) { return _to GetContentType(aContentType); } \
  NS_IMETHOD SetContentType(const nsACString & aContentType) { return _to SetContentType(aContentType); } \
  NS_IMETHOD GetContentCharset(nsACString & aContentCharset) { return _to GetContentCharset(aContentCharset); } \
  NS_IMETHOD SetContentCharset(const nsACString & aContentCharset) { return _to SetContentCharset(aContentCharset); } \
  NS_IMETHOD GetContentLength(PRInt32 *aContentLength) { return _to GetContentLength(aContentLength); } \
  NS_IMETHOD SetContentLength(PRInt32 aContentLength) { return _to SetContentLength(aContentLength); } \
  NS_IMETHOD Open(nsIInputStream **_retval) { return _to Open(_retval); } \
  NS_IMETHOD AsyncOpen(nsIStreamListener *listener, nsISupports *xcontext) { return _to AsyncOpen(listener, xcontext); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICHANNEL(_to) \
  NS_IMETHOD GetOriginalURI(nsIURI * *aOriginalURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOriginalURI(aOriginalURI); } \
  NS_IMETHOD SetOriginalURI(nsIURI * aOriginalURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetOriginalURI(aOriginalURI); } \
  NS_IMETHOD GetURI(nsIURI * *aURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetURI(aURI); } \
  NS_IMETHOD GetOwner(nsISupports * *aOwner) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOwner(aOwner); } \
  NS_IMETHOD SetOwner(nsISupports * aOwner) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetOwner(aOwner); } \
  NS_IMETHOD GetNotificationCallbacks(nsIInterfaceRequestor * *aNotificationCallbacks) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNotificationCallbacks(aNotificationCallbacks); } \
  NS_IMETHOD SetNotificationCallbacks(nsIInterfaceRequestor * aNotificationCallbacks) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetNotificationCallbacks(aNotificationCallbacks); } \
  NS_IMETHOD GetSecurityInfo(nsISupports * *aSecurityInfo) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSecurityInfo(aSecurityInfo); } \
  NS_IMETHOD GetContentType(nsACString & aContentType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetContentType(aContentType); } \
  NS_IMETHOD SetContentType(const nsACString & aContentType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetContentType(aContentType); } \
  NS_IMETHOD GetContentCharset(nsACString & aContentCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetContentCharset(aContentCharset); } \
  NS_IMETHOD SetContentCharset(const nsACString & aContentCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetContentCharset(aContentCharset); } \
  NS_IMETHOD GetContentLength(PRInt32 *aContentLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetContentLength(aContentLength); } \
  NS_IMETHOD SetContentLength(PRInt32 aContentLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetContentLength(aContentLength); } \
  NS_IMETHOD Open(nsIInputStream **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Open(_retval); } \
  NS_IMETHOD AsyncOpen(nsIStreamListener *listener, nsISupports *xcontext) { return !_to ? NS_ERROR_NULL_POINTER : _to->AsyncOpen(listener, xcontext); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsChannel : public nsIChannel
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICHANNEL

  nsChannel();

private:
  ~nsChannel();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsChannel, nsIChannel)

nsChannel::nsChannel()
{
  /* member initializers and constructor code */
}

nsChannel::~nsChannel()
{
  /* destructor code */
}

/* attribute nsIURI originalURI; */
NS_IMETHODIMP nsChannel::GetOriginalURI(nsIURI * *aOriginalURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsChannel::SetOriginalURI(nsIURI * aOriginalURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIURI URI; */
NS_IMETHODIMP nsChannel::GetURI(nsIURI * *aURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsISupports owner; */
NS_IMETHODIMP nsChannel::GetOwner(nsISupports * *aOwner)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsChannel::SetOwner(nsISupports * aOwner)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIInterfaceRequestor notificationCallbacks; */
NS_IMETHODIMP nsChannel::GetNotificationCallbacks(nsIInterfaceRequestor * *aNotificationCallbacks)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsChannel::SetNotificationCallbacks(nsIInterfaceRequestor * aNotificationCallbacks)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsISupports securityInfo; */
NS_IMETHODIMP nsChannel::GetSecurityInfo(nsISupports * *aSecurityInfo)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute ACString contentType; */
NS_IMETHODIMP nsChannel::GetContentType(nsACString & aContentType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsChannel::SetContentType(const nsACString & aContentType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute ACString contentCharset; */
NS_IMETHODIMP nsChannel::GetContentCharset(nsACString & aContentCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsChannel::SetContentCharset(const nsACString & aContentCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long contentLength; */
NS_IMETHODIMP nsChannel::GetContentLength(PRInt32 *aContentLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsChannel::SetContentLength(PRInt32 aContentLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIInputStream open (); */
NS_IMETHODIMP nsChannel::Open(nsIInputStream **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void asyncOpen (in nsIStreamListener listener, in nsISupports xcontext); */
NS_IMETHODIMP nsChannel::AsyncOpen(nsIStreamListener *listener, nsISupports *xcontext)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIClipboardCommands */
#define NS_ICLIPBOARDCOMMANDS_IID_STR "b8100c90-73be-11d2-92a5-00105a1b0d64"

#define NS_ICLIPBOARDCOMMANDS_IID \
  {0xb8100c90, 0x73be, 0x11d2, \
    { 0x92, 0xa5, 0x00, 0x10, 0x5a, 0x1b, 0x0d, 0x64 }}

class NS_NO_VTABLE nsIClipboardCommands : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICLIPBOARDCOMMANDS_IID)

  /* boolean canCutSelection (); */
  NS_IMETHOD CanCutSelection(PRBool *_retval) = 0;

  /* boolean canCopySelection (); */
  NS_IMETHOD CanCopySelection(PRBool *_retval) = 0;

  /* boolean canCopyLinkLocation (); */
  NS_IMETHOD CanCopyLinkLocation(PRBool *_retval) = 0;

  /* boolean canCopyImageLocation (); */
  NS_IMETHOD CanCopyImageLocation(PRBool *_retval) = 0;

  /* boolean canCopyImageContents (); */
  NS_IMETHOD CanCopyImageContents(PRBool *_retval) = 0;

  /* boolean canPaste (); */
  NS_IMETHOD CanPaste(PRBool *_retval) = 0;

  /* void cutSelection (); */
  NS_IMETHOD CutSelection(void) = 0;

  /* void copySelection (); */
  NS_IMETHOD CopySelection(void) = 0;

  /* void copyLinkLocation (); */
  NS_IMETHOD CopyLinkLocation(void) = 0;

  /* void copyImageLocation (); */
  NS_IMETHOD CopyImageLocation(void) = 0;

  /* void copyImageContents (); */
  NS_IMETHOD CopyImageContents(void) = 0;

  /* void paste (); */
  NS_IMETHOD Paste(void) = 0;

  /* void selectAll (); */
  NS_IMETHOD SelectAll(void) = 0;

  /* void selectNone (); */
  NS_IMETHOD SelectNone(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICLIPBOARDCOMMANDS \
  NS_IMETHOD CanCutSelection(PRBool *_retval); \
  NS_IMETHOD CanCopySelection(PRBool *_retval); \
  NS_IMETHOD CanCopyLinkLocation(PRBool *_retval); \
  NS_IMETHOD CanCopyImageLocation(PRBool *_retval); \
  NS_IMETHOD CanCopyImageContents(PRBool *_retval); \
  NS_IMETHOD CanPaste(PRBool *_retval); \
  NS_IMETHOD CutSelection(void); \
  NS_IMETHOD CopySelection(void); \
  NS_IMETHOD CopyLinkLocation(void); \
  NS_IMETHOD CopyImageLocation(void); \
  NS_IMETHOD CopyImageContents(void); \
  NS_IMETHOD Paste(void); \
  NS_IMETHOD SelectAll(void); \
  NS_IMETHOD SelectNone(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICLIPBOARDCOMMANDS(_to) \
  NS_IMETHOD CanCutSelection(PRBool *_retval) { return _to CanCutSelection(_retval); } \
  NS_IMETHOD CanCopySelection(PRBool *_retval) { return _to CanCopySelection(_retval); } \
  NS_IMETHOD CanCopyLinkLocation(PRBool *_retval) { return _to CanCopyLinkLocation(_retval); } \
  NS_IMETHOD CanCopyImageLocation(PRBool *_retval) { return _to CanCopyImageLocation(_retval); } \
  NS_IMETHOD CanCopyImageContents(PRBool *_retval) { return _to CanCopyImageContents(_retval); } \
  NS_IMETHOD CanPaste(PRBool *_retval) { return _to CanPaste(_retval); } \
  NS_IMETHOD CutSelection(void) { return _to CutSelection(); } \
  NS_IMETHOD CopySelection(void) { return _to CopySelection(); } \
  NS_IMETHOD CopyLinkLocation(void) { return _to CopyLinkLocation(); } \
  NS_IMETHOD CopyImageLocation(void) { return _to CopyImageLocation(); } \
  NS_IMETHOD CopyImageContents(void) { return _to CopyImageContents(); } \
  NS_IMETHOD Paste(void) { return _to Paste(); } \
  NS_IMETHOD SelectAll(void) { return _to SelectAll(); } \
  NS_IMETHOD SelectNone(void) { return _to SelectNone(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICLIPBOARDCOMMANDS(_to) \
  NS_IMETHOD CanCutSelection(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanCutSelection(_retval); } \
  NS_IMETHOD CanCopySelection(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanCopySelection(_retval); } \
  NS_IMETHOD CanCopyLinkLocation(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanCopyLinkLocation(_retval); } \
  NS_IMETHOD CanCopyImageLocation(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanCopyImageLocation(_retval); } \
  NS_IMETHOD CanCopyImageContents(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanCopyImageContents(_retval); } \
  NS_IMETHOD CanPaste(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanPaste(_retval); } \
  NS_IMETHOD CutSelection(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CutSelection(); } \
  NS_IMETHOD CopySelection(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopySelection(); } \
  NS_IMETHOD CopyLinkLocation(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyLinkLocation(); } \
  NS_IMETHOD CopyImageLocation(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyImageLocation(); } \
  NS_IMETHOD CopyImageContents(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyImageContents(); } \
  NS_IMETHOD Paste(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Paste(); } \
  NS_IMETHOD SelectAll(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->SelectAll(); } \
  NS_IMETHOD SelectNone(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->SelectNone(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsClipboardCommands : public nsIClipboardCommands
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICLIPBOARDCOMMANDS

  nsClipboardCommands();

private:
  ~nsClipboardCommands();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsClipboardCommands, nsIClipboardCommands)

nsClipboardCommands::nsClipboardCommands()
{
  /* member initializers and constructor code */
}

nsClipboardCommands::~nsClipboardCommands()
{
  /* destructor code */
}

/* boolean canCutSelection (); */
NS_IMETHODIMP nsClipboardCommands::CanCutSelection(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean canCopySelection (); */
NS_IMETHODIMP nsClipboardCommands::CanCopySelection(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean canCopyLinkLocation (); */
NS_IMETHODIMP nsClipboardCommands::CanCopyLinkLocation(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean canCopyImageLocation (); */
NS_IMETHODIMP nsClipboardCommands::CanCopyImageLocation(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean canCopyImageContents (); */
NS_IMETHODIMP nsClipboardCommands::CanCopyImageContents(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean canPaste (); */
NS_IMETHODIMP nsClipboardCommands::CanPaste(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void cutSelection (); */
NS_IMETHODIMP nsClipboardCommands::CutSelection()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void copySelection (); */
NS_IMETHODIMP nsClipboardCommands::CopySelection()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void copyLinkLocation (); */
NS_IMETHODIMP nsClipboardCommands::CopyLinkLocation()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void copyImageLocation (); */
NS_IMETHODIMP nsClipboardCommands::CopyImageLocation()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void copyImageContents (); */
NS_IMETHODIMP nsClipboardCommands::CopyImageContents()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void paste (); */
NS_IMETHODIMP nsClipboardCommands::Paste()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void selectAll (); */
NS_IMETHODIMP nsClipboardCommands::SelectAll()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void selectNone (); */
NS_IMETHODIMP nsClipboardCommands::SelectNone()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIComponentManager */
#define NS_ICOMPONENTMANAGER_IID_STR "a88e5a60-205a-4bb1-94e1-2628daf51eae"

#define NS_ICOMPONENTMANAGER_IID \
  {0xa88e5a60, 0x205a, 0x4bb1, \
    { 0x94, 0xe1, 0x26, 0x28, 0xda, 0xf5, 0x1e, 0xae }}

class NS_NO_VTABLE nsIComponentManager : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICOMPONENTMANAGER_IID)

  /* void getClassObject (in nsCIDRef class_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD GetClassObject(const nsCID & class_id, const nsIID & iid, void * *result) = 0;

  /* void getClassObjectByContractID (in string contract_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD GetClassObjectByContractID(const char *contract_id, const nsIID & iid, void * *result) = 0;

  /* void createInstance (in nsCIDRef class_id, in nsISupports delegate, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD CreateInstance(const nsCID & class_id, nsISupports *delegate, const nsIID & iid, void * *result) = 0;

  /* void createInstanceByContractID (in string contract_id, in nsISupports delegate, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD CreateInstanceByContractID(const char *contract_id, nsISupports *delegate, const nsIID & iid, void * *result) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICOMPONENTMANAGER \
  NS_IMETHOD GetClassObject(const nsCID & class_id, const nsIID & iid, void * *result); \
  NS_IMETHOD GetClassObjectByContractID(const char *contract_id, const nsIID & iid, void * *result); \
  NS_IMETHOD CreateInstance(const nsCID & class_id, nsISupports *delegate, const nsIID & iid, void * *result); \
  NS_IMETHOD CreateInstanceByContractID(const char *contract_id, nsISupports *delegate, const nsIID & iid, void * *result); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICOMPONENTMANAGER(_to) \
  NS_IMETHOD GetClassObject(const nsCID & class_id, const nsIID & iid, void * *result) { return _to GetClassObject(class_id, iid, result); } \
  NS_IMETHOD GetClassObjectByContractID(const char *contract_id, const nsIID & iid, void * *result) { return _to GetClassObjectByContractID(contract_id, iid, result); } \
  NS_IMETHOD CreateInstance(const nsCID & class_id, nsISupports *delegate, const nsIID & iid, void * *result) { return _to CreateInstance(class_id, delegate, iid, result); } \
  NS_IMETHOD CreateInstanceByContractID(const char *contract_id, nsISupports *delegate, const nsIID & iid, void * *result) { return _to CreateInstanceByContractID(contract_id, delegate, iid, result); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICOMPONENTMANAGER(_to) \
  NS_IMETHOD GetClassObject(const nsCID & class_id, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetClassObject(class_id, iid, result); } \
  NS_IMETHOD GetClassObjectByContractID(const char *contract_id, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetClassObjectByContractID(contract_id, iid, result); } \
  NS_IMETHOD CreateInstance(const nsCID & class_id, nsISupports *delegate, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateInstance(class_id, delegate, iid, result); } \
  NS_IMETHOD CreateInstanceByContractID(const char *contract_id, nsISupports *delegate, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateInstanceByContractID(contract_id, delegate, iid, result); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsComponentManager : public nsIComponentManager
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICOMPONENTMANAGER

  nsComponentManager();

private:
  ~nsComponentManager();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsComponentManager, nsIComponentManager)

nsComponentManager::nsComponentManager()
{
  /* member initializers and constructor code */
}

nsComponentManager::~nsComponentManager()
{
  /* destructor code */
}

/* void getClassObject (in nsCIDRef class_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsComponentManager::GetClassObject(const nsCID & class_id, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getClassObjectByContractID (in string contract_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsComponentManager::GetClassObjectByContractID(const char *contract_id, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void createInstance (in nsCIDRef class_id, in nsISupports delegate, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsComponentManager::CreateInstance(const nsCID & class_id, nsISupports *delegate, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void createInstanceByContractID (in string contract_id, in nsISupports delegate, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsComponentManager::CreateInstanceByContractID(const char *contract_id, nsISupports *delegate, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIFactory; /* forward declaration */

class nsISimpleEnumerator; /* forward declaration */

class nsIFile; /* forward declaration */


/* starting interface:    nsIComponentRegistrar */
#define NS_ICOMPONENTREGISTRAR_IID_STR "2417cbfe-65ad-48a6-b4b6-eb84db174392"

#define NS_ICOMPONENTREGISTRAR_IID \
  {0x2417cbfe, 0x65ad, 0x48a6, \
    { 0xb4, 0xb6, 0xeb, 0x84, 0xdb, 0x17, 0x43, 0x92 }}

class NS_NO_VTABLE nsIComponentRegistrar : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICOMPONENTREGISTRAR_IID)

  /* void autoRegister (in nsIFile spec); */
  NS_IMETHOD AutoRegister(nsIFile *spec) = 0;

  /* void autoUnregister (in nsIFile spec); */
  NS_IMETHOD AutoUnregister(nsIFile *spec) = 0;

  /* void registerFactory (in nsCIDRef class_id, in string class_name, in string contract_id, in nsIFactory factory); */
  NS_IMETHOD RegisterFactory(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFactory *factory) = 0;

  /* void unregisterFactory (in nsCIDRef class_id, in nsIFactory factory); */
  NS_IMETHOD UnregisterFactory(const nsCID & class_id, nsIFactory *factory) = 0;

  /* void registerFactoryLocation (in nsCIDRef class_id, in string class_name, in string contract_id, in nsIFile file, in string loader_str, in string type); */
  NS_IMETHOD RegisterFactoryLocation(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFile *file, const char *loader_str, const char *type) = 0;

  /* void unregisterFactoryLocation (in nsCIDRef class_id, in nsIFile file); */
  NS_IMETHOD UnregisterFactoryLocation(const nsCID & class_id, nsIFile *file) = 0;

  /* boolean isCIDRegistered (in nsCIDRef class_id); */
  NS_IMETHOD IsCIDRegistered(const nsCID & class_id, PRBool *_retval) = 0;

  /* boolean isContractIDRegistered (in string contract_id); */
  NS_IMETHOD IsContractIDRegistered(const char *contract_id, PRBool *_retval) = 0;

  /* nsISimpleEnumerator enumerateCIDs (); */
  NS_IMETHOD EnumerateCIDs(nsISimpleEnumerator **_retval) = 0;

  /* nsISimpleEnumerator enumerateContractIDs (); */
  NS_IMETHOD EnumerateContractIDs(nsISimpleEnumerator **_retval) = 0;

  /* string CIDToContractID (in nsCIDRef class_id); */
  NS_IMETHOD CIDToContractID(const nsCID & class_id, char **_retval) = 0;

  /* nsCIDPtr contractIDToCID (in string contract_id); */
  NS_IMETHOD ContractIDToCID(const char *contract_id, nsCID * *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICOMPONENTREGISTRAR \
  NS_IMETHOD AutoRegister(nsIFile *spec); \
  NS_IMETHOD AutoUnregister(nsIFile *spec); \
  NS_IMETHOD RegisterFactory(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFactory *factory); \
  NS_IMETHOD UnregisterFactory(const nsCID & class_id, nsIFactory *factory); \
  NS_IMETHOD RegisterFactoryLocation(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFile *file, const char *loader_str, const char *type); \
  NS_IMETHOD UnregisterFactoryLocation(const nsCID & class_id, nsIFile *file); \
  NS_IMETHOD IsCIDRegistered(const nsCID & class_id, PRBool *_retval); \
  NS_IMETHOD IsContractIDRegistered(const char *contract_id, PRBool *_retval); \
  NS_IMETHOD EnumerateCIDs(nsISimpleEnumerator **_retval); \
  NS_IMETHOD EnumerateContractIDs(nsISimpleEnumerator **_retval); \
  NS_IMETHOD CIDToContractID(const nsCID & class_id, char **_retval); \
  NS_IMETHOD ContractIDToCID(const char *contract_id, nsCID * *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICOMPONENTREGISTRAR(_to) \
  NS_IMETHOD AutoRegister(nsIFile *spec) { return _to AutoRegister(spec); } \
  NS_IMETHOD AutoUnregister(nsIFile *spec) { return _to AutoUnregister(spec); } \
  NS_IMETHOD RegisterFactory(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFactory *factory) { return _to RegisterFactory(class_id, class_name, contract_id, factory); } \
  NS_IMETHOD UnregisterFactory(const nsCID & class_id, nsIFactory *factory) { return _to UnregisterFactory(class_id, factory); } \
  NS_IMETHOD RegisterFactoryLocation(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFile *file, const char *loader_str, const char *type) { return _to RegisterFactoryLocation(class_id, class_name, contract_id, file, loader_str, type); } \
  NS_IMETHOD UnregisterFactoryLocation(const nsCID & class_id, nsIFile *file) { return _to UnregisterFactoryLocation(class_id, file); } \
  NS_IMETHOD IsCIDRegistered(const nsCID & class_id, PRBool *_retval) { return _to IsCIDRegistered(class_id, _retval); } \
  NS_IMETHOD IsContractIDRegistered(const char *contract_id, PRBool *_retval) { return _to IsContractIDRegistered(contract_id, _retval); } \
  NS_IMETHOD EnumerateCIDs(nsISimpleEnumerator **_retval) { return _to EnumerateCIDs(_retval); } \
  NS_IMETHOD EnumerateContractIDs(nsISimpleEnumerator **_retval) { return _to EnumerateContractIDs(_retval); } \
  NS_IMETHOD CIDToContractID(const nsCID & class_id, char **_retval) { return _to CIDToContractID(class_id, _retval); } \
  NS_IMETHOD ContractIDToCID(const char *contract_id, nsCID * *_retval) { return _to ContractIDToCID(contract_id, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICOMPONENTREGISTRAR(_to) \
  NS_IMETHOD AutoRegister(nsIFile *spec) { return !_to ? NS_ERROR_NULL_POINTER : _to->AutoRegister(spec); } \
  NS_IMETHOD AutoUnregister(nsIFile *spec) { return !_to ? NS_ERROR_NULL_POINTER : _to->AutoUnregister(spec); } \
  NS_IMETHOD RegisterFactory(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFactory *factory) { return !_to ? NS_ERROR_NULL_POINTER : _to->RegisterFactory(class_id, class_name, contract_id, factory); } \
  NS_IMETHOD UnregisterFactory(const nsCID & class_id, nsIFactory *factory) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnregisterFactory(class_id, factory); } \
  NS_IMETHOD RegisterFactoryLocation(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFile *file, const char *loader_str, const char *type) { return !_to ? NS_ERROR_NULL_POINTER : _to->RegisterFactoryLocation(class_id, class_name, contract_id, file, loader_str, type); } \
  NS_IMETHOD UnregisterFactoryLocation(const nsCID & class_id, nsIFile *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnregisterFactoryLocation(class_id, file); } \
  NS_IMETHOD IsCIDRegistered(const nsCID & class_id, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsCIDRegistered(class_id, _retval); } \
  NS_IMETHOD IsContractIDRegistered(const char *contract_id, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsContractIDRegistered(contract_id, _retval); } \
  NS_IMETHOD EnumerateCIDs(nsISimpleEnumerator **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EnumerateCIDs(_retval); } \
  NS_IMETHOD EnumerateContractIDs(nsISimpleEnumerator **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->EnumerateContractIDs(_retval); } \
  NS_IMETHOD CIDToContractID(const nsCID & class_id, char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CIDToContractID(class_id, _retval); } \
  NS_IMETHOD ContractIDToCID(const char *contract_id, nsCID * *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ContractIDToCID(contract_id, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsComponentRegistrar : public nsIComponentRegistrar
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICOMPONENTREGISTRAR

  nsComponentRegistrar();

private:
  ~nsComponentRegistrar();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsComponentRegistrar, nsIComponentRegistrar)

nsComponentRegistrar::nsComponentRegistrar()
{
  /* member initializers and constructor code */
}

nsComponentRegistrar::~nsComponentRegistrar()
{
  /* destructor code */
}

/* void autoRegister (in nsIFile spec); */
NS_IMETHODIMP nsComponentRegistrar::AutoRegister(nsIFile *spec)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void autoUnregister (in nsIFile spec); */
NS_IMETHODIMP nsComponentRegistrar::AutoUnregister(nsIFile *spec)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void registerFactory (in nsCIDRef class_id, in string class_name, in string contract_id, in nsIFactory factory); */
NS_IMETHODIMP nsComponentRegistrar::RegisterFactory(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFactory *factory)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void unregisterFactory (in nsCIDRef class_id, in nsIFactory factory); */
NS_IMETHODIMP nsComponentRegistrar::UnregisterFactory(const nsCID & class_id, nsIFactory *factory)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void registerFactoryLocation (in nsCIDRef class_id, in string class_name, in string contract_id, in nsIFile file, in string loader_str, in string type); */
NS_IMETHODIMP nsComponentRegistrar::RegisterFactoryLocation(const nsCID & class_id, const char *class_name, const char *contract_id, nsIFile *file, const char *loader_str, const char *type)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void unregisterFactoryLocation (in nsCIDRef class_id, in nsIFile file); */
NS_IMETHODIMP nsComponentRegistrar::UnregisterFactoryLocation(const nsCID & class_id, nsIFile *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isCIDRegistered (in nsCIDRef class_id); */
NS_IMETHODIMP nsComponentRegistrar::IsCIDRegistered(const nsCID & class_id, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isContractIDRegistered (in string contract_id); */
NS_IMETHODIMP nsComponentRegistrar::IsContractIDRegistered(const char *contract_id, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsISimpleEnumerator enumerateCIDs (); */
NS_IMETHODIMP nsComponentRegistrar::EnumerateCIDs(nsISimpleEnumerator **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsISimpleEnumerator enumerateContractIDs (); */
NS_IMETHODIMP nsComponentRegistrar::EnumerateContractIDs(nsISimpleEnumerator **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string CIDToContractID (in nsCIDRef class_id); */
NS_IMETHODIMP nsComponentRegistrar::CIDToContractID(const nsCID & class_id, char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsCIDPtr contractIDToCID (in string contract_id); */
NS_IMETHODIMP nsComponentRegistrar::ContractIDToCID(const char *contract_id, nsCID * *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIContextMenuInfo; /* forward declaration */

class nsIDOMEvent; /* forward declaration */

class nsIDOMNode; /* forward declaration */

class nsIURI; /* forward declaration */

class imgIContainer; /* forward declaration */


/* starting interface:    nsIContextMenuListener2 */
#define NS_ICONTEXTMENULISTENER2_IID_STR "7fb719b3-d804-4964-9596-77cf924ee314"

#define NS_ICONTEXTMENULISTENER2_IID \
  {0x7fb719b3, 0xd804, 0x4964, \
    { 0x95, 0x96, 0x77, 0xcf, 0x92, 0x4e, 0xe3, 0x14 }}

class NS_NO_VTABLE nsIContextMenuListener2 : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICONTEXTMENULISTENER2_IID)

  /* void onShowContextMenu (in unsigned long context_flags, in nsIContextMenuInfo utils); */
  NS_IMETHOD OnShowContextMenu(PRUint32 context_flags, nsIContextMenuInfo *utils) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICONTEXTMENULISTENER2 \
  NS_IMETHOD OnShowContextMenu(PRUint32 context_flags, nsIContextMenuInfo *utils); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICONTEXTMENULISTENER2(_to) \
  NS_IMETHOD OnShowContextMenu(PRUint32 context_flags, nsIContextMenuInfo *utils) { return _to OnShowContextMenu(context_flags, utils); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICONTEXTMENULISTENER2(_to) \
  NS_IMETHOD OnShowContextMenu(PRUint32 context_flags, nsIContextMenuInfo *utils) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnShowContextMenu(context_flags, utils); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsContextMenuListener2 : public nsIContextMenuListener2
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICONTEXTMENULISTENER2

  nsContextMenuListener2();

private:
  ~nsContextMenuListener2();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsContextMenuListener2, nsIContextMenuListener2)

nsContextMenuListener2::nsContextMenuListener2()
{
  /* member initializers and constructor code */
}

nsContextMenuListener2::~nsContextMenuListener2()
{
  /* destructor code */
}

/* void onShowContextMenu (in unsigned long context_flags, in nsIContextMenuInfo utils); */
NS_IMETHODIMP nsContextMenuListener2::OnShowContextMenu(PRUint32 context_flags, nsIContextMenuInfo *utils)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIContextMenuInfo */
#define NS_ICONTEXTMENUINFO_IID_STR "2f977d56-5485-11d4-87e2-0010a4e75ef2"

#define NS_ICONTEXTMENUINFO_IID \
  {0x2f977d56, 0x5485, 0x11d4, \
    { 0x87, 0xe2, 0x00, 0x10, 0xa4, 0xe7, 0x5e, 0xf2 }}

class NS_NO_VTABLE nsIContextMenuInfo : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICONTEXTMENUINFO_IID)

  /* readonly attribute nsIDOMEvent mouseEvent; */
  NS_IMETHOD GetMouseEvent(nsIDOMEvent * *aMouseEvent) = 0;

  /* readonly attribute nsIDOMNode targetNode; */
  NS_IMETHOD GetTargetNode(nsIDOMNode * *aTargetNode) = 0;

  /* readonly attribute AString associatedLink; */
  NS_IMETHOD GetAssociatedLink(nsAString & aAssociatedLink) = 0;

  /* readonly attribute imgIContainer imageContainer; */
  NS_IMETHOD GetImageContainer(imgIContainer * *aImageContainer) = 0;

  /* readonly attribute nsIURI imageSrc; */
  NS_IMETHOD GetImageSrc(nsIURI * *aImageSrc) = 0;

  /* readonly attribute imgIContainer backgroundImageContainer; */
  NS_IMETHOD GetBackgroundImageContainer(imgIContainer * *aBackgroundImageContainer) = 0;

  /* readonly attribute nsIURI backgroundImageSrc; */
  NS_IMETHOD GetBackgroundImageSrc(nsIURI * *aBackgroundImageSrc) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICONTEXTMENUINFO \
  NS_IMETHOD GetMouseEvent(nsIDOMEvent * *aMouseEvent); \
  NS_IMETHOD GetTargetNode(nsIDOMNode * *aTargetNode); \
  NS_IMETHOD GetAssociatedLink(nsAString & aAssociatedLink); \
  NS_IMETHOD GetImageContainer(imgIContainer * *aImageContainer); \
  NS_IMETHOD GetImageSrc(nsIURI * *aImageSrc); \
  NS_IMETHOD GetBackgroundImageContainer(imgIContainer * *aBackgroundImageContainer); \
  NS_IMETHOD GetBackgroundImageSrc(nsIURI * *aBackgroundImageSrc); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICONTEXTMENUINFO(_to) \
  NS_IMETHOD GetMouseEvent(nsIDOMEvent * *aMouseEvent) { return _to GetMouseEvent(aMouseEvent); } \
  NS_IMETHOD GetTargetNode(nsIDOMNode * *aTargetNode) { return _to GetTargetNode(aTargetNode); } \
  NS_IMETHOD GetAssociatedLink(nsAString & aAssociatedLink) { return _to GetAssociatedLink(aAssociatedLink); } \
  NS_IMETHOD GetImageContainer(imgIContainer * *aImageContainer) { return _to GetImageContainer(aImageContainer); } \
  NS_IMETHOD GetImageSrc(nsIURI * *aImageSrc) { return _to GetImageSrc(aImageSrc); } \
  NS_IMETHOD GetBackgroundImageContainer(imgIContainer * *aBackgroundImageContainer) { return _to GetBackgroundImageContainer(aBackgroundImageContainer); } \
  NS_IMETHOD GetBackgroundImageSrc(nsIURI * *aBackgroundImageSrc) { return _to GetBackgroundImageSrc(aBackgroundImageSrc); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICONTEXTMENUINFO(_to) \
  NS_IMETHOD GetMouseEvent(nsIDOMEvent * *aMouseEvent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMouseEvent(aMouseEvent); } \
  NS_IMETHOD GetTargetNode(nsIDOMNode * *aTargetNode) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTargetNode(aTargetNode); } \
  NS_IMETHOD GetAssociatedLink(nsAString & aAssociatedLink) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAssociatedLink(aAssociatedLink); } \
  NS_IMETHOD GetImageContainer(imgIContainer * *aImageContainer) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetImageContainer(aImageContainer); } \
  NS_IMETHOD GetImageSrc(nsIURI * *aImageSrc) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetImageSrc(aImageSrc); } \
  NS_IMETHOD GetBackgroundImageContainer(imgIContainer * *aBackgroundImageContainer) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBackgroundImageContainer(aBackgroundImageContainer); } \
  NS_IMETHOD GetBackgroundImageSrc(nsIURI * *aBackgroundImageSrc) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBackgroundImageSrc(aBackgroundImageSrc); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsContextMenuInfo : public nsIContextMenuInfo
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICONTEXTMENUINFO

  nsContextMenuInfo();

private:
  ~nsContextMenuInfo();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsContextMenuInfo, nsIContextMenuInfo)

nsContextMenuInfo::nsContextMenuInfo()
{
  /* member initializers and constructor code */
}

nsContextMenuInfo::~nsContextMenuInfo()
{
  /* destructor code */
}

/* readonly attribute nsIDOMEvent mouseEvent; */
NS_IMETHODIMP nsContextMenuInfo::GetMouseEvent(nsIDOMEvent * *aMouseEvent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNode targetNode; */
NS_IMETHODIMP nsContextMenuInfo::GetTargetNode(nsIDOMNode * *aTargetNode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString associatedLink; */
NS_IMETHODIMP nsContextMenuInfo::GetAssociatedLink(nsAString & aAssociatedLink)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute imgIContainer imageContainer; */
NS_IMETHODIMP nsContextMenuInfo::GetImageContainer(imgIContainer * *aImageContainer)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIURI imageSrc; */
NS_IMETHODIMP nsContextMenuInfo::GetImageSrc(nsIURI * *aImageSrc)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute imgIContainer backgroundImageContainer; */
NS_IMETHODIMP nsContextMenuInfo::GetBackgroundImageContainer(imgIContainer * *aBackgroundImageContainer)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIURI backgroundImageSrc; */
NS_IMETHODIMP nsContextMenuInfo::GetBackgroundImageSrc(nsIURI * *aBackgroundImageSrc)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIFile; /* forward declaration */

class nsISimpleEnumerator; /* forward declaration */


/* starting interface:    nsIDirectoryServiceProvider */
#define NS_IDIRECTORYSERVICEPROVIDER_IID_STR "bbf8cab0-d43a-11d3-8cc2-00609792278c"

#define NS_IDIRECTORYSERVICEPROVIDER_IID \
  {0xbbf8cab0, 0xd43a, 0x11d3, \
    { 0x8c, 0xc2, 0x00, 0x60, 0x97, 0x92, 0x27, 0x8c }}

class NS_NO_VTABLE nsIDirectoryServiceProvider : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDIRECTORYSERVICEPROVIDER_IID)

  /* nsIFile getFile (in string property, out boolean persistent); */
  NS_IMETHOD GetFile(const char *property, PRBool *persistent, nsIFile **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDIRECTORYSERVICEPROVIDER \
  NS_IMETHOD GetFile(const char *property, PRBool *persistent, nsIFile **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDIRECTORYSERVICEPROVIDER(_to) \
  NS_IMETHOD GetFile(const char *property, PRBool *persistent, nsIFile **_retval) { return _to GetFile(property, persistent, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDIRECTORYSERVICEPROVIDER(_to) \
  NS_IMETHOD GetFile(const char *property, PRBool *persistent, nsIFile **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFile(property, persistent, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDirectoryServiceProvider : public nsIDirectoryServiceProvider
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDIRECTORYSERVICEPROVIDER

  nsDirectoryServiceProvider();

private:
  ~nsDirectoryServiceProvider();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDirectoryServiceProvider, nsIDirectoryServiceProvider)

nsDirectoryServiceProvider::nsDirectoryServiceProvider()
{
  /* member initializers and constructor code */
}

nsDirectoryServiceProvider::~nsDirectoryServiceProvider()
{
  /* destructor code */
}

/* nsIFile getFile (in string property, out boolean persistent); */
NS_IMETHODIMP nsDirectoryServiceProvider::GetFile(const char *property, PRBool *persistent, nsIFile **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDirectoryServiceProvider2 */
#define NS_IDIRECTORYSERVICEPROVIDER2_IID_STR "2f977d4b-5485-11d4-87e2-0010a4e75ef2"

#define NS_IDIRECTORYSERVICEPROVIDER2_IID \
  {0x2f977d4b, 0x5485, 0x11d4, \
    { 0x87, 0xe2, 0x00, 0x10, 0xa4, 0xe7, 0x5e, 0xf2 }}

class NS_NO_VTABLE nsIDirectoryServiceProvider2 : public nsIDirectoryServiceProvider {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDIRECTORYSERVICEPROVIDER2_IID)

  /* nsISimpleEnumerator getFiles (in string property); */
  NS_IMETHOD GetFiles(const char *property, nsISimpleEnumerator **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDIRECTORYSERVICEPROVIDER2 \
  NS_IMETHOD GetFiles(const char *property, nsISimpleEnumerator **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDIRECTORYSERVICEPROVIDER2(_to) \
  NS_IMETHOD GetFiles(const char *property, nsISimpleEnumerator **_retval) { return _to GetFiles(property, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDIRECTORYSERVICEPROVIDER2(_to) \
  NS_IMETHOD GetFiles(const char *property, nsISimpleEnumerator **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFiles(property, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDirectoryServiceProvider2 : public nsIDirectoryServiceProvider2
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDIRECTORYSERVICEPROVIDER2

  nsDirectoryServiceProvider2();

private:
  ~nsDirectoryServiceProvider2();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDirectoryServiceProvider2, nsIDirectoryServiceProvider2)

nsDirectoryServiceProvider2::nsDirectoryServiceProvider2()
{
  /* member initializers and constructor code */
}

nsDirectoryServiceProvider2::~nsDirectoryServiceProvider2()
{
  /* destructor code */
}

/* nsISimpleEnumerator getFiles (in string property); */
NS_IMETHODIMP nsDirectoryServiceProvider2::GetFiles(const char *property, nsISimpleEnumerator **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDirectoryService */
#define NS_IDIRECTORYSERVICE_IID_STR "57a66a60-d43a-11d3-8cc2-00609792278c"

#define NS_IDIRECTORYSERVICE_IID \
  {0x57a66a60, 0xd43a, 0x11d3, \
    { 0x8c, 0xc2, 0x00, 0x60, 0x97, 0x92, 0x27, 0x8c }}

class NS_NO_VTABLE nsIDirectoryService : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDIRECTORYSERVICE_IID)

  /* void init (); */
  NS_IMETHOD Init(void) = 0;

  /* void registerProvider (in nsIDirectoryServiceProvider provider); */
  NS_IMETHOD RegisterProvider(nsIDirectoryServiceProvider *provider) = 0;

  /* void unregisterProvider (in nsIDirectoryServiceProvider provider); */
  NS_IMETHOD UnregisterProvider(nsIDirectoryServiceProvider *provider) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDIRECTORYSERVICE \
  NS_IMETHOD Init(void); \
  NS_IMETHOD RegisterProvider(nsIDirectoryServiceProvider *provider); \
  NS_IMETHOD UnregisterProvider(nsIDirectoryServiceProvider *provider); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDIRECTORYSERVICE(_to) \
  NS_IMETHOD Init(void) { return _to Init(); } \
  NS_IMETHOD RegisterProvider(nsIDirectoryServiceProvider *provider) { return _to RegisterProvider(provider); } \
  NS_IMETHOD UnregisterProvider(nsIDirectoryServiceProvider *provider) { return _to UnregisterProvider(provider); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDIRECTORYSERVICE(_to) \
  NS_IMETHOD Init(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Init(); } \
  NS_IMETHOD RegisterProvider(nsIDirectoryServiceProvider *provider) { return !_to ? NS_ERROR_NULL_POINTER : _to->RegisterProvider(provider); } \
  NS_IMETHOD UnregisterProvider(nsIDirectoryServiceProvider *provider) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnregisterProvider(provider); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDirectoryService : public nsIDirectoryService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDIRECTORYSERVICE

  nsDirectoryService();

private:
  ~nsDirectoryService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDirectoryService, nsIDirectoryService)

nsDirectoryService::nsDirectoryService()
{
  /* member initializers and constructor code */
}

nsDirectoryService::~nsDirectoryService()
{
  /* destructor code */
}

/* void init (); */
NS_IMETHODIMP nsDirectoryService::Init()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void registerProvider (in nsIDirectoryServiceProvider provider); */
NS_IMETHODIMP nsDirectoryService::RegisterProvider(nsIDirectoryServiceProvider *provider)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void unregisterProvider (in nsIDirectoryServiceProvider provider); */
NS_IMETHODIMP nsDirectoryService::UnregisterProvider(nsIDirectoryServiceProvider *provider)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDocShellTreeOwner; /* forward declaration */

class nsIDocShellTreeItem; /* forward declaration */


/* starting interface:    nsIDocShellTreeNode */
#define NS_IDOCSHELLTREENODE_IID_STR "37f1ab73-f224-44b1-82f0-d2834ab1cec0"

#define NS_IDOCSHELLTREENODE_IID \
  {0x37f1ab73, 0xf224, 0x44b1, \
    { 0x82, 0xf0, 0xd2, 0x83, 0x4a, 0xb1, 0xce, 0xc0 }}

class NS_NO_VTABLE nsIDocShellTreeNode : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOCSHELLTREENODE_IID)

  /* readonly attribute long childCount; */
  NS_IMETHOD GetChildCount(PRInt32 *aChildCount) = 0;

  /* void addChild (in nsIDocShellTreeItem child); */
  NS_IMETHOD AddChild(nsIDocShellTreeItem *child) = 0;

  /* void removeChild (in nsIDocShellTreeItem child); */
  NS_IMETHOD RemoveChild(nsIDocShellTreeItem *child) = 0;

  /* nsIDocShellTreeItem getChildAt (in long index); */
  NS_IMETHOD GetChildAt(PRInt32 index, nsIDocShellTreeItem **_retval) = 0;

  /* nsIDocShellTreeItem findChildWithName (in wstring name, in boolean recurse, in boolean same_type, in nsIDocShellTreeItem requestor, in nsIDocShellTreeItem original_requestor); */
  NS_IMETHOD FindChildWithName(const PRUnichar *name, PRBool recurse, PRBool same_type, nsIDocShellTreeItem *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOCSHELLTREENODE \
  NS_IMETHOD GetChildCount(PRInt32 *aChildCount); \
  NS_IMETHOD AddChild(nsIDocShellTreeItem *child); \
  NS_IMETHOD RemoveChild(nsIDocShellTreeItem *child); \
  NS_IMETHOD GetChildAt(PRInt32 index, nsIDocShellTreeItem **_retval); \
  NS_IMETHOD FindChildWithName(const PRUnichar *name, PRBool recurse, PRBool same_type, nsIDocShellTreeItem *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOCSHELLTREENODE(_to) \
  NS_IMETHOD GetChildCount(PRInt32 *aChildCount) { return _to GetChildCount(aChildCount); } \
  NS_IMETHOD AddChild(nsIDocShellTreeItem *child) { return _to AddChild(child); } \
  NS_IMETHOD RemoveChild(nsIDocShellTreeItem *child) { return _to RemoveChild(child); } \
  NS_IMETHOD GetChildAt(PRInt32 index, nsIDocShellTreeItem **_retval) { return _to GetChildAt(index, _retval); } \
  NS_IMETHOD FindChildWithName(const PRUnichar *name, PRBool recurse, PRBool same_type, nsIDocShellTreeItem *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) { return _to FindChildWithName(name, recurse, same_type, requestor, original_requestor, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOCSHELLTREENODE(_to) \
  NS_IMETHOD GetChildCount(PRInt32 *aChildCount) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChildCount(aChildCount); } \
  NS_IMETHOD AddChild(nsIDocShellTreeItem *child) { return !_to ? NS_ERROR_NULL_POINTER : _to->AddChild(child); } \
  NS_IMETHOD RemoveChild(nsIDocShellTreeItem *child) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveChild(child); } \
  NS_IMETHOD GetChildAt(PRInt32 index, nsIDocShellTreeItem **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChildAt(index, _retval); } \
  NS_IMETHOD FindChildWithName(const PRUnichar *name, PRBool recurse, PRBool same_type, nsIDocShellTreeItem *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->FindChildWithName(name, recurse, same_type, requestor, original_requestor, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDocShellTreeNode : public nsIDocShellTreeNode
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOCSHELLTREENODE

  nsDocShellTreeNode();

private:
  ~nsDocShellTreeNode();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDocShellTreeNode, nsIDocShellTreeNode)

nsDocShellTreeNode::nsDocShellTreeNode()
{
  /* member initializers and constructor code */
}

nsDocShellTreeNode::~nsDocShellTreeNode()
{
  /* destructor code */
}

/* readonly attribute long childCount; */
NS_IMETHODIMP nsDocShellTreeNode::GetChildCount(PRInt32 *aChildCount)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void addChild (in nsIDocShellTreeItem child); */
NS_IMETHODIMP nsDocShellTreeNode::AddChild(nsIDocShellTreeItem *child)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeChild (in nsIDocShellTreeItem child); */
NS_IMETHODIMP nsDocShellTreeNode::RemoveChild(nsIDocShellTreeItem *child)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDocShellTreeItem getChildAt (in long index); */
NS_IMETHODIMP nsDocShellTreeNode::GetChildAt(PRInt32 index, nsIDocShellTreeItem **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDocShellTreeItem findChildWithName (in wstring name, in boolean recurse, in boolean same_type, in nsIDocShellTreeItem requestor, in nsIDocShellTreeItem original_requestor); */
NS_IMETHODIMP nsDocShellTreeNode::FindChildWithName(const PRUnichar *name, PRBool recurse, PRBool same_type, nsIDocShellTreeItem *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDocShellTreeItem */
#define NS_IDOCSHELLTREEITEM_IID_STR "09b54ec1-d98a-49a9-bc95-3219e8b55089"

#define NS_IDOCSHELLTREEITEM_IID \
  {0x09b54ec1, 0xd98a, 0x49a9, \
    { 0xbc, 0x95, 0x32, 0x19, 0xe8, 0xb5, 0x50, 0x89 }}

class NS_NO_VTABLE nsIDocShellTreeItem : public nsIDocShellTreeNode {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOCSHELLTREEITEM_IID)

  /* attribute wstring name; */
  NS_IMETHOD GetName(PRUnichar * *aName) = 0;
  NS_IMETHOD SetName(const PRUnichar * aName) = 0;

  /* boolean nameEquals (in wstring name); */
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval) = 0;

  enum { typeChrome = 0 };

  enum { typeContent = 1 };

  enum { typeContentWrapper = 2 };

  enum { typeChromeWrapper = 3 };

  enum { typeAll = 2147483647 };

  /* attribute long itemType; */
  NS_IMETHOD GetItemType(PRInt32 *aItemType) = 0;
  NS_IMETHOD SetItemType(PRInt32 aItemType) = 0;

  /* readonly attribute nsIDocShellTreeItem parent; */
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent) = 0;

  /* readonly attribute nsIDocShellTreeItem sameTypeParent; */
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent) = 0;

  /* readonly attribute nsIDocShellTreeItem rootTreeItem; */
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem) = 0;

  /* readonly attribute nsIDocShellTreeItem sameTypeRootTreeItem; */
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem) = 0;

  /* nsIDocShellTreeItem findItemWithName (in wstring name, in nsISupports requestor, in nsIDocShellTreeItem original_requestor); */
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) = 0;

  /* readonly attribute nsIDocShellTreeOwner treeOwner; */
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner) = 0;

  /* [noscript] void setTreeOwner (in nsIDocShellTreeOwner tree_owner); */
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOCSHELLTREEITEM \
  NS_IMETHOD GetName(PRUnichar * *aName); \
  NS_IMETHOD SetName(const PRUnichar * aName); \
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval); \
  NS_IMETHOD GetItemType(PRInt32 *aItemType); \
  NS_IMETHOD SetItemType(PRInt32 aItemType); \
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent); \
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent); \
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem); \
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem); \
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval); \
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner); \
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOCSHELLTREEITEM(_to) \
  NS_IMETHOD GetName(PRUnichar * *aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const PRUnichar * aName) { return _to SetName(aName); } \
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval) { return _to NameEquals(name, _retval); } \
  NS_IMETHOD GetItemType(PRInt32 *aItemType) { return _to GetItemType(aItemType); } \
  NS_IMETHOD SetItemType(PRInt32 aItemType) { return _to SetItemType(aItemType); } \
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent) { return _to GetParent(aParent); } \
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent) { return _to GetSameTypeParent(aSameTypeParent); } \
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem) { return _to GetRootTreeItem(aRootTreeItem); } \
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem) { return _to GetSameTypeRootTreeItem(aSameTypeRootTreeItem); } \
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) { return _to FindItemWithName(name, requestor, original_requestor, _retval); } \
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner) { return _to GetTreeOwner(aTreeOwner); } \
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner) { return _to SetTreeOwner(tree_owner); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOCSHELLTREEITEM(_to) \
  NS_IMETHOD GetName(PRUnichar * *aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const PRUnichar * aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->NameEquals(name, _retval); } \
  NS_IMETHOD GetItemType(PRInt32 *aItemType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetItemType(aItemType); } \
  NS_IMETHOD SetItemType(PRInt32 aItemType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetItemType(aItemType); } \
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParent(aParent); } \
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSameTypeParent(aSameTypeParent); } \
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRootTreeItem(aRootTreeItem); } \
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSameTypeRootTreeItem(aSameTypeRootTreeItem); } \
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->FindItemWithName(name, requestor, original_requestor, _retval); } \
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTreeOwner(aTreeOwner); } \
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTreeOwner(tree_owner); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDocShellTreeItem : public nsIDocShellTreeItem
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOCSHELLTREEITEM

  nsDocShellTreeItem();

private:
  ~nsDocShellTreeItem();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDocShellTreeItem, nsIDocShellTreeItem)

nsDocShellTreeItem::nsDocShellTreeItem()
{
  /* member initializers and constructor code */
}

nsDocShellTreeItem::~nsDocShellTreeItem()
{
  /* destructor code */
}

/* attribute wstring name; */
NS_IMETHODIMP nsDocShellTreeItem::GetName(PRUnichar * *aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDocShellTreeItem::SetName(const PRUnichar * aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean nameEquals (in wstring name); */
NS_IMETHODIMP nsDocShellTreeItem::NameEquals(const PRUnichar *name, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long itemType; */
NS_IMETHODIMP nsDocShellTreeItem::GetItemType(PRInt32 *aItemType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDocShellTreeItem::SetItemType(PRInt32 aItemType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem parent; */
NS_IMETHODIMP nsDocShellTreeItem::GetParent(nsIDocShellTreeItem * *aParent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem sameTypeParent; */
NS_IMETHODIMP nsDocShellTreeItem::GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem rootTreeItem; */
NS_IMETHODIMP nsDocShellTreeItem::GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem sameTypeRootTreeItem; */
NS_IMETHODIMP nsDocShellTreeItem::GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDocShellTreeItem findItemWithName (in wstring name, in nsISupports requestor, in nsIDocShellTreeItem original_requestor); */
NS_IMETHODIMP nsDocShellTreeItem::FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeOwner treeOwner; */
NS_IMETHODIMP nsDocShellTreeItem::GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void setTreeOwner (in nsIDocShellTreeOwner tree_owner); */
NS_IMETHODIMP nsDocShellTreeItem::SetTreeOwner(nsIDocShellTreeOwner *tree_owner)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    ns18IDocShellTreeItem */
#define NS_18IDOCSHELLTREEITEM_IID_STR "7d935d63-6d2a-4600-afb5-9a4f7d68b825"

#define NS_18IDOCSHELLTREEITEM_IID \
  {0x7d935d63, 0x6d2a, 0x4600, \
    { 0xaf, 0xb5, 0x9a, 0x4f, 0x7d, 0x68, 0xb8, 0x25 }}

class NS_NO_VTABLE ns18IDocShellTreeItem : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_18IDOCSHELLTREEITEM_IID)

  /* attribute wstring name; */
  NS_IMETHOD GetName(PRUnichar * *aName) = 0;
  NS_IMETHOD SetName(const PRUnichar * aName) = 0;

  /* boolean nameEquals (in wstring name); */
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval) = 0;

  enum { typeChrome = 0 };

  enum { typeContent = 1 };

  enum { typeContentWrapper = 2 };

  enum { typeChromeWrapper = 3 };

  enum { typeAll = 2147483647 };

  /* attribute long itemType; */
  NS_IMETHOD GetItemType(PRInt32 *aItemType) = 0;
  NS_IMETHOD SetItemType(PRInt32 aItemType) = 0;

  /* readonly attribute nsIDocShellTreeItem parent; */
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent) = 0;

  /* readonly attribute nsIDocShellTreeItem sameTypeParent; */
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent) = 0;

  /* readonly attribute nsIDocShellTreeItem rootTreeItem; */
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem) = 0;

  /* readonly attribute nsIDocShellTreeItem sameTypeRootTreeItem; */
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem) = 0;

  /* nsIDocShellTreeItem findItemWithName (in wstring name, in nsISupports requestor, in nsIDocShellTreeItem original_requestor); */
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) = 0;

  /* readonly attribute nsIDocShellTreeOwner treeOwner; */
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner) = 0;

  /* [noscript] void setTreeOwner (in nsIDocShellTreeOwner tree_owner); */
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner) = 0;

  /* attribute long childOffset; */
  NS_IMETHOD GetChildOffset(PRInt32 *aChildOffset) = 0;
  NS_IMETHOD SetChildOffset(PRInt32 aChildOffset) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NS18IDOCSHELLTREEITEM \
  NS_IMETHOD GetName(PRUnichar * *aName); \
  NS_IMETHOD SetName(const PRUnichar * aName); \
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval); \
  NS_IMETHOD GetItemType(PRInt32 *aItemType); \
  NS_IMETHOD SetItemType(PRInt32 aItemType); \
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent); \
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent); \
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem); \
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem); \
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval); \
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner); \
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner); \
  NS_IMETHOD GetChildOffset(PRInt32 *aChildOffset); \
  NS_IMETHOD SetChildOffset(PRInt32 aChildOffset); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NS18IDOCSHELLTREEITEM(_to) \
  NS_IMETHOD GetName(PRUnichar * *aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const PRUnichar * aName) { return _to SetName(aName); } \
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval) { return _to NameEquals(name, _retval); } \
  NS_IMETHOD GetItemType(PRInt32 *aItemType) { return _to GetItemType(aItemType); } \
  NS_IMETHOD SetItemType(PRInt32 aItemType) { return _to SetItemType(aItemType); } \
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent) { return _to GetParent(aParent); } \
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent) { return _to GetSameTypeParent(aSameTypeParent); } \
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem) { return _to GetRootTreeItem(aRootTreeItem); } \
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem) { return _to GetSameTypeRootTreeItem(aSameTypeRootTreeItem); } \
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) { return _to FindItemWithName(name, requestor, original_requestor, _retval); } \
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner) { return _to GetTreeOwner(aTreeOwner); } \
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner) { return _to SetTreeOwner(tree_owner); } \
  NS_IMETHOD GetChildOffset(PRInt32 *aChildOffset) { return _to GetChildOffset(aChildOffset); } \
  NS_IMETHOD SetChildOffset(PRInt32 aChildOffset) { return _to SetChildOffset(aChildOffset); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NS18IDOCSHELLTREEITEM(_to) \
  NS_IMETHOD GetName(PRUnichar * *aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const PRUnichar * aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD NameEquals(const PRUnichar *name, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->NameEquals(name, _retval); } \
  NS_IMETHOD GetItemType(PRInt32 *aItemType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetItemType(aItemType); } \
  NS_IMETHOD SetItemType(PRInt32 aItemType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetItemType(aItemType); } \
  NS_IMETHOD GetParent(nsIDocShellTreeItem * *aParent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParent(aParent); } \
  NS_IMETHOD GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSameTypeParent(aSameTypeParent); } \
  NS_IMETHOD GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRootTreeItem(aRootTreeItem); } \
  NS_IMETHOD GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSameTypeRootTreeItem(aSameTypeRootTreeItem); } \
  NS_IMETHOD FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->FindItemWithName(name, requestor, original_requestor, _retval); } \
  NS_IMETHOD GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTreeOwner(aTreeOwner); } \
  NS_IMETHOD SetTreeOwner(nsIDocShellTreeOwner *tree_owner) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTreeOwner(tree_owner); } \
  NS_IMETHOD GetChildOffset(PRInt32 *aChildOffset) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChildOffset(aChildOffset); } \
  NS_IMETHOD SetChildOffset(PRInt32 aChildOffset) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetChildOffset(aChildOffset); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public ns18IDocShellTreeItem
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NS18IDOCSHELLTREEITEM

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, ns18IDocShellTreeItem)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* attribute wstring name; */
NS_IMETHODIMP _MYCLASS_::GetName(PRUnichar * *aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP _MYCLASS_::SetName(const PRUnichar * aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean nameEquals (in wstring name); */
NS_IMETHODIMP _MYCLASS_::NameEquals(const PRUnichar *name, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long itemType; */
NS_IMETHODIMP _MYCLASS_::GetItemType(PRInt32 *aItemType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP _MYCLASS_::SetItemType(PRInt32 aItemType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem parent; */
NS_IMETHODIMP _MYCLASS_::GetParent(nsIDocShellTreeItem * *aParent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem sameTypeParent; */
NS_IMETHODIMP _MYCLASS_::GetSameTypeParent(nsIDocShellTreeItem * *aSameTypeParent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem rootTreeItem; */
NS_IMETHODIMP _MYCLASS_::GetRootTreeItem(nsIDocShellTreeItem * *aRootTreeItem)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeItem sameTypeRootTreeItem; */
NS_IMETHODIMP _MYCLASS_::GetSameTypeRootTreeItem(nsIDocShellTreeItem * *aSameTypeRootTreeItem)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDocShellTreeItem findItemWithName (in wstring name, in nsISupports requestor, in nsIDocShellTreeItem original_requestor); */
NS_IMETHODIMP _MYCLASS_::FindItemWithName(const PRUnichar *name, nsISupports *requestor, nsIDocShellTreeItem *original_requestor, nsIDocShellTreeItem **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDocShellTreeOwner treeOwner; */
NS_IMETHODIMP _MYCLASS_::GetTreeOwner(nsIDocShellTreeOwner * *aTreeOwner)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void setTreeOwner (in nsIDocShellTreeOwner tree_owner); */
NS_IMETHODIMP _MYCLASS_::SetTreeOwner(nsIDocShellTreeOwner *tree_owner)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long childOffset; */
NS_IMETHODIMP _MYCLASS_::GetChildOffset(PRInt32 *aChildOffset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP _MYCLASS_::SetChildOffset(PRInt32 aChildOffset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMNodeList; /* forward declaration */

class nsIDOMNamedNodeMap; /* forward declaration */

class nsIDOMDocument; /* forward declaration */

class DOMException; /* forward declaration */


/* starting interface:    nsIDOMNode */
#define NS_IDOMNODE_IID_STR "a6cf907c-15b3-11d2-932e-00805f8add32"

#define NS_IDOMNODE_IID \
  {0xa6cf907c, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMNode : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMNODE_IID)

  enum { ELEMENT_NODE = 1U };

  enum { ATTRIBUTE_NODE = 2U };

  enum { TEXT_NODE = 3U };

  enum { CDATA_SECTION_NODE = 4U };

  enum { ENTITY_REFERENCE_NODE = 5U };

  enum { ENTITY_NODE = 6U };

  enum { PROCESSING_INSTRUCTION_NODE = 7U };

  enum { COMMENT_NODE = 8U };

  enum { DOCUMENT_NODE = 9U };

  enum { DOCUMENT_TYPE_NODE = 10U };

  enum { DOCUMENT_FRAGMENT_NODE = 11U };

  enum { NOTATION_NODE = 12U };

  /* readonly attribute DOMString nodeName; */
  NS_IMETHOD GetNodeName(nsAString & aNodeName) = 0;

  /* attribute DOMString nodeValue; */
  NS_IMETHOD GetNodeValue(nsAString & aNodeValue) = 0;
  NS_IMETHOD SetNodeValue(const nsAString & aNodeValue) = 0;

  /* readonly attribute unsigned short nodeType; */
  NS_IMETHOD GetNodeType(PRUint16 *aNodeType) = 0;

  /* readonly attribute nsIDOMNode parentNode; */
  NS_IMETHOD GetParentNode(nsIDOMNode * *aParentNode) = 0;

  /* readonly attribute nsIDOMNodeList childNodes; */
  NS_IMETHOD GetChildNodes(nsIDOMNodeList * *aChildNodes) = 0;

  /* readonly attribute nsIDOMNode firstChild; */
  NS_IMETHOD GetFirstChild(nsIDOMNode * *aFirstChild) = 0;

  /* readonly attribute nsIDOMNode lastChild; */
  NS_IMETHOD GetLastChild(nsIDOMNode * *aLastChild) = 0;

  /* readonly attribute nsIDOMNode previousSibling; */
  NS_IMETHOD GetPreviousSibling(nsIDOMNode * *aPreviousSibling) = 0;

  /* readonly attribute nsIDOMNode nextSibling; */
  NS_IMETHOD GetNextSibling(nsIDOMNode * *aNextSibling) = 0;

  /* readonly attribute nsIDOMNamedNodeMap attributes; */
  NS_IMETHOD GetAttributes(nsIDOMNamedNodeMap * *aAttributes) = 0;

  /* readonly attribute nsIDOMDocument ownerDocument; */
  NS_IMETHOD GetOwnerDocument(nsIDOMDocument * *aOwnerDocument) = 0;

  /* nsIDOMNode insertBefore (in nsIDOMNode new_child, in nsIDOMNode ref_child)  raises (DOMException); */
  NS_IMETHOD InsertBefore(nsIDOMNode *new_child, nsIDOMNode *ref_child, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode replaceChild (in nsIDOMNode new_child, in nsIDOMNode old_child)  raises (DOMException); */
  NS_IMETHOD ReplaceChild(nsIDOMNode *new_child, nsIDOMNode *old_child, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode removeChild (in nsIDOMNode old_child)  raises (DOMException); */
  NS_IMETHOD RemoveChild(nsIDOMNode *old_child, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode appendChild (in nsIDOMNode new_child)  raises (DOMException); */
  NS_IMETHOD AppendChild(nsIDOMNode *new_child, nsIDOMNode **_retval) = 0;

  /* boolean hasChildNodes (); */
  NS_IMETHOD HasChildNodes(PRBool *_retval) = 0;

  /* nsIDOMNode cloneNode (in boolean deep); */
  NS_IMETHOD CloneNode(PRBool deep, nsIDOMNode **_retval) = 0;

  /* void normalize (); */
  NS_IMETHOD Normalize(void) = 0;

  /* boolean isSupported (in DOMString feature, in DOMString version); */
  NS_IMETHOD IsSupported(const nsAString & feature, const nsAString & version, PRBool *_retval) = 0;

  /* readonly attribute DOMString namespaceURI; */
  NS_IMETHOD GetNamespaceURI(nsAString & aNamespaceURI) = 0;

  /* attribute DOMString prefix; */
  NS_IMETHOD GetPrefix(nsAString & aPrefix) = 0;
  NS_IMETHOD SetPrefix(const nsAString & aPrefix) = 0;

  /* readonly attribute DOMString localName; */
  NS_IMETHOD GetLocalName(nsAString & aLocalName) = 0;

  /* boolean hasAttributes (); */
  NS_IMETHOD HasAttributes(PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMNODE \
  NS_IMETHOD GetNodeName(nsAString & aNodeName); \
  NS_IMETHOD GetNodeValue(nsAString & aNodeValue); \
  NS_IMETHOD SetNodeValue(const nsAString & aNodeValue); \
  NS_IMETHOD GetNodeType(PRUint16 *aNodeType); \
  NS_IMETHOD GetParentNode(nsIDOMNode * *aParentNode); \
  NS_IMETHOD GetChildNodes(nsIDOMNodeList * *aChildNodes); \
  NS_IMETHOD GetFirstChild(nsIDOMNode * *aFirstChild); \
  NS_IMETHOD GetLastChild(nsIDOMNode * *aLastChild); \
  NS_IMETHOD GetPreviousSibling(nsIDOMNode * *aPreviousSibling); \
  NS_IMETHOD GetNextSibling(nsIDOMNode * *aNextSibling); \
  NS_IMETHOD GetAttributes(nsIDOMNamedNodeMap * *aAttributes); \
  NS_IMETHOD GetOwnerDocument(nsIDOMDocument * *aOwnerDocument); \
  NS_IMETHOD InsertBefore(nsIDOMNode *new_child, nsIDOMNode *ref_child, nsIDOMNode **_retval); \
  NS_IMETHOD ReplaceChild(nsIDOMNode *new_child, nsIDOMNode *old_child, nsIDOMNode **_retval); \
  NS_IMETHOD RemoveChild(nsIDOMNode *old_child, nsIDOMNode **_retval); \
  NS_IMETHOD AppendChild(nsIDOMNode *new_child, nsIDOMNode **_retval); \
  NS_IMETHOD HasChildNodes(PRBool *_retval); \
  NS_IMETHOD CloneNode(PRBool deep, nsIDOMNode **_retval); \
  NS_IMETHOD Normalize(void); \
  NS_IMETHOD IsSupported(const nsAString & feature, const nsAString & version, PRBool *_retval); \
  NS_IMETHOD GetNamespaceURI(nsAString & aNamespaceURI); \
  NS_IMETHOD GetPrefix(nsAString & aPrefix); \
  NS_IMETHOD SetPrefix(const nsAString & aPrefix); \
  NS_IMETHOD GetLocalName(nsAString & aLocalName); \
  NS_IMETHOD HasAttributes(PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMNODE(_to) \
  NS_IMETHOD GetNodeName(nsAString & aNodeName) { return _to GetNodeName(aNodeName); } \
  NS_IMETHOD GetNodeValue(nsAString & aNodeValue) { return _to GetNodeValue(aNodeValue); } \
  NS_IMETHOD SetNodeValue(const nsAString & aNodeValue) { return _to SetNodeValue(aNodeValue); } \
  NS_IMETHOD GetNodeType(PRUint16 *aNodeType) { return _to GetNodeType(aNodeType); } \
  NS_IMETHOD GetParentNode(nsIDOMNode * *aParentNode) { return _to GetParentNode(aParentNode); } \
  NS_IMETHOD GetChildNodes(nsIDOMNodeList * *aChildNodes) { return _to GetChildNodes(aChildNodes); } \
  NS_IMETHOD GetFirstChild(nsIDOMNode * *aFirstChild) { return _to GetFirstChild(aFirstChild); } \
  NS_IMETHOD GetLastChild(nsIDOMNode * *aLastChild) { return _to GetLastChild(aLastChild); } \
  NS_IMETHOD GetPreviousSibling(nsIDOMNode * *aPreviousSibling) { return _to GetPreviousSibling(aPreviousSibling); } \
  NS_IMETHOD GetNextSibling(nsIDOMNode * *aNextSibling) { return _to GetNextSibling(aNextSibling); } \
  NS_IMETHOD GetAttributes(nsIDOMNamedNodeMap * *aAttributes) { return _to GetAttributes(aAttributes); } \
  NS_IMETHOD GetOwnerDocument(nsIDOMDocument * *aOwnerDocument) { return _to GetOwnerDocument(aOwnerDocument); } \
  NS_IMETHOD InsertBefore(nsIDOMNode *new_child, nsIDOMNode *ref_child, nsIDOMNode **_retval) { return _to InsertBefore(new_child, ref_child, _retval); } \
  NS_IMETHOD ReplaceChild(nsIDOMNode *new_child, nsIDOMNode *old_child, nsIDOMNode **_retval) { return _to ReplaceChild(new_child, old_child, _retval); } \
  NS_IMETHOD RemoveChild(nsIDOMNode *old_child, nsIDOMNode **_retval) { return _to RemoveChild(old_child, _retval); } \
  NS_IMETHOD AppendChild(nsIDOMNode *new_child, nsIDOMNode **_retval) { return _to AppendChild(new_child, _retval); } \
  NS_IMETHOD HasChildNodes(PRBool *_retval) { return _to HasChildNodes(_retval); } \
  NS_IMETHOD CloneNode(PRBool deep, nsIDOMNode **_retval) { return _to CloneNode(deep, _retval); } \
  NS_IMETHOD Normalize(void) { return _to Normalize(); } \
  NS_IMETHOD IsSupported(const nsAString & feature, const nsAString & version, PRBool *_retval) { return _to IsSupported(feature, version, _retval); } \
  NS_IMETHOD GetNamespaceURI(nsAString & aNamespaceURI) { return _to GetNamespaceURI(aNamespaceURI); } \
  NS_IMETHOD GetPrefix(nsAString & aPrefix) { return _to GetPrefix(aPrefix); } \
  NS_IMETHOD SetPrefix(const nsAString & aPrefix) { return _to SetPrefix(aPrefix); } \
  NS_IMETHOD GetLocalName(nsAString & aLocalName) { return _to GetLocalName(aLocalName); } \
  NS_IMETHOD HasAttributes(PRBool *_retval) { return _to HasAttributes(_retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMNODE(_to) \
  NS_IMETHOD GetNodeName(nsAString & aNodeName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNodeName(aNodeName); } \
  NS_IMETHOD GetNodeValue(nsAString & aNodeValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNodeValue(aNodeValue); } \
  NS_IMETHOD SetNodeValue(const nsAString & aNodeValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetNodeValue(aNodeValue); } \
  NS_IMETHOD GetNodeType(PRUint16 *aNodeType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNodeType(aNodeType); } \
  NS_IMETHOD GetParentNode(nsIDOMNode * *aParentNode) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParentNode(aParentNode); } \
  NS_IMETHOD GetChildNodes(nsIDOMNodeList * *aChildNodes) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChildNodes(aChildNodes); } \
  NS_IMETHOD GetFirstChild(nsIDOMNode * *aFirstChild) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFirstChild(aFirstChild); } \
  NS_IMETHOD GetLastChild(nsIDOMNode * *aLastChild) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastChild(aLastChild); } \
  NS_IMETHOD GetPreviousSibling(nsIDOMNode * *aPreviousSibling) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPreviousSibling(aPreviousSibling); } \
  NS_IMETHOD GetNextSibling(nsIDOMNode * *aNextSibling) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNextSibling(aNextSibling); } \
  NS_IMETHOD GetAttributes(nsIDOMNamedNodeMap * *aAttributes) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAttributes(aAttributes); } \
  NS_IMETHOD GetOwnerDocument(nsIDOMDocument * *aOwnerDocument) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOwnerDocument(aOwnerDocument); } \
  NS_IMETHOD InsertBefore(nsIDOMNode *new_child, nsIDOMNode *ref_child, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->InsertBefore(new_child, ref_child, _retval); } \
  NS_IMETHOD ReplaceChild(nsIDOMNode *new_child, nsIDOMNode *old_child, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReplaceChild(new_child, old_child, _retval); } \
  NS_IMETHOD RemoveChild(nsIDOMNode *old_child, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveChild(old_child, _retval); } \
  NS_IMETHOD AppendChild(nsIDOMNode *new_child, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->AppendChild(new_child, _retval); } \
  NS_IMETHOD HasChildNodes(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->HasChildNodes(_retval); } \
  NS_IMETHOD CloneNode(PRBool deep, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CloneNode(deep, _retval); } \
  NS_IMETHOD Normalize(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Normalize(); } \
  NS_IMETHOD IsSupported(const nsAString & feature, const nsAString & version, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsSupported(feature, version, _retval); } \
  NS_IMETHOD GetNamespaceURI(nsAString & aNamespaceURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNamespaceURI(aNamespaceURI); } \
  NS_IMETHOD GetPrefix(nsAString & aPrefix) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrefix(aPrefix); } \
  NS_IMETHOD SetPrefix(const nsAString & aPrefix) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrefix(aPrefix); } \
  NS_IMETHOD GetLocalName(nsAString & aLocalName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLocalName(aLocalName); } \
  NS_IMETHOD HasAttributes(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->HasAttributes(_retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMNode : public nsIDOMNode
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMNODE

  nsDOMNode();

private:
  ~nsDOMNode();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMNode, nsIDOMNode)

nsDOMNode::nsDOMNode()
{
  /* member initializers and constructor code */
}

nsDOMNode::~nsDOMNode()
{
  /* destructor code */
}

/* readonly attribute DOMString nodeName; */
NS_IMETHODIMP nsDOMNode::GetNodeName(nsAString & aNodeName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString nodeValue; */
NS_IMETHODIMP nsDOMNode::GetNodeValue(nsAString & aNodeValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMNode::SetNodeValue(const nsAString & aNodeValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned short nodeType; */
NS_IMETHODIMP nsDOMNode::GetNodeType(PRUint16 *aNodeType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNode parentNode; */
NS_IMETHODIMP nsDOMNode::GetParentNode(nsIDOMNode * *aParentNode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNodeList childNodes; */
NS_IMETHODIMP nsDOMNode::GetChildNodes(nsIDOMNodeList * *aChildNodes)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNode firstChild; */
NS_IMETHODIMP nsDOMNode::GetFirstChild(nsIDOMNode * *aFirstChild)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNode lastChild; */
NS_IMETHODIMP nsDOMNode::GetLastChild(nsIDOMNode * *aLastChild)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNode previousSibling; */
NS_IMETHODIMP nsDOMNode::GetPreviousSibling(nsIDOMNode * *aPreviousSibling)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNode nextSibling; */
NS_IMETHODIMP nsDOMNode::GetNextSibling(nsIDOMNode * *aNextSibling)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMNamedNodeMap attributes; */
NS_IMETHODIMP nsDOMNode::GetAttributes(nsIDOMNamedNodeMap * *aAttributes)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMDocument ownerDocument; */
NS_IMETHODIMP nsDOMNode::GetOwnerDocument(nsIDOMDocument * *aOwnerDocument)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode insertBefore (in nsIDOMNode new_child, in nsIDOMNode ref_child)  raises (DOMException); */
NS_IMETHODIMP nsDOMNode::InsertBefore(nsIDOMNode *new_child, nsIDOMNode *ref_child, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode replaceChild (in nsIDOMNode new_child, in nsIDOMNode old_child)  raises (DOMException); */
NS_IMETHODIMP nsDOMNode::ReplaceChild(nsIDOMNode *new_child, nsIDOMNode *old_child, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode removeChild (in nsIDOMNode old_child)  raises (DOMException); */
NS_IMETHODIMP nsDOMNode::RemoveChild(nsIDOMNode *old_child, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode appendChild (in nsIDOMNode new_child)  raises (DOMException); */
NS_IMETHODIMP nsDOMNode::AppendChild(nsIDOMNode *new_child, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean hasChildNodes (); */
NS_IMETHODIMP nsDOMNode::HasChildNodes(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode cloneNode (in boolean deep); */
NS_IMETHODIMP nsDOMNode::CloneNode(PRBool deep, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void normalize (); */
NS_IMETHODIMP nsDOMNode::Normalize()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isSupported (in DOMString feature, in DOMString version); */
NS_IMETHODIMP nsDOMNode::IsSupported(const nsAString & feature, const nsAString & version, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute DOMString namespaceURI; */
NS_IMETHODIMP nsDOMNode::GetNamespaceURI(nsAString & aNamespaceURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString prefix; */
NS_IMETHODIMP nsDOMNode::GetPrefix(nsAString & aPrefix)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMNode::SetPrefix(const nsAString & aPrefix)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute DOMString localName; */
NS_IMETHODIMP nsDOMNode::GetLocalName(nsAString & aLocalName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean hasAttributes (); */
NS_IMETHODIMP nsDOMNode::HasAttributes(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMNode; /* forward declaration */

class DOMException; /* forward declaration */

class nsIVariant; /* forward declaration */

class nsIDOMUserDataHandler; /* forward declaration */


/* starting interface:    nsIDOM3Node */
#define NS_IDOM3NODE_IID_STR "29fb2a18-1dd2-11b2-8dd9-a6fd5d5ad12f"

#define NS_IDOM3NODE_IID \
  {0x29fb2a18, 0x1dd2, 0x11b2, \
    { 0x8d, 0xd9, 0xa6, 0xfd, 0x5d, 0x5a, 0xd1, 0x2f }}

class NS_NO_VTABLE nsIDOM3Node : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOM3NODE_IID)

  enum { DOCUMENT_POSITION_DISCONNECTED = 1U };

  enum { DOCUMENT_POSITION_PRECEDING = 2U };

  enum { DOCUMENT_POSITION_FOLLOWING = 4U };

  enum { DOCUMENT_POSITION_CONTAINS = 8U };

  enum { DOCUMENT_POSITION_CONTAINED_BY = 16U };

  enum { DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 32U };

  /* readonly attribute DOMString baseURI; */
  NS_IMETHOD GetBaseURI(nsAString & aBaseURI) = 0;

  /* unsigned short compareDocumentPosition (in nsIDOMNode other)  raises (DOMException); */
  NS_IMETHOD CompareDocumentPosition(nsIDOMNode *other, PRUint16 *_retval) = 0;

  /* attribute DOMString textContent; */
  NS_IMETHOD GetTextContent(nsAString & aTextContent) = 0;
  NS_IMETHOD SetTextContent(const nsAString & aTextContent) = 0;

  /* boolean isSameNode (in nsIDOMNode other); */
  NS_IMETHOD IsSameNode(nsIDOMNode *other, PRBool *_retval) = 0;

  /* DOMString lookupPrefix (in DOMString namespaceURI); */
  NS_IMETHOD LookupPrefix(const nsAString & namespaceURI, nsAString & _retval) = 0;

  /* boolean isDefaultNamespace (in DOMString namespaceURI); */
  NS_IMETHOD IsDefaultNamespace(const nsAString & namespaceURI, PRBool *_retval) = 0;

  /* DOMString lookupNamespaceURI (in DOMString prefix); */
  NS_IMETHOD LookupNamespaceURI(const nsAString & prefix, nsAString & _retval) = 0;

  /* boolean isEqualNode (in nsIDOMNode arg); */
  NS_IMETHOD IsEqualNode(nsIDOMNode *arg, PRBool *_retval) = 0;

  /* nsISupports getFeature (in DOMString feature, in DOMString version); */
  NS_IMETHOD GetFeature(const nsAString & feature, const nsAString & version, nsISupports **_retval) = 0;

  /* nsIVariant setUserData (in DOMString key, in nsIVariant data, in nsIDOMUserDataHandler handler); */
  NS_IMETHOD SetUserData(const nsAString & key, nsIVariant *data, nsIDOMUserDataHandler *handler, nsIVariant **_retval) = 0;

  /* nsIVariant getUserData (in DOMString key); */
  NS_IMETHOD GetUserData(const nsAString & key, nsIVariant **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOM3NODE \
  NS_IMETHOD GetBaseURI(nsAString & aBaseURI); \
  NS_IMETHOD CompareDocumentPosition(nsIDOMNode *other, PRUint16 *_retval); \
  NS_IMETHOD GetTextContent(nsAString & aTextContent); \
  NS_IMETHOD SetTextContent(const nsAString & aTextContent); \
  NS_IMETHOD IsSameNode(nsIDOMNode *other, PRBool *_retval); \
  NS_IMETHOD LookupPrefix(const nsAString & namespaceURI, nsAString & _retval); \
  NS_IMETHOD IsDefaultNamespace(const nsAString & namespaceURI, PRBool *_retval); \
  NS_IMETHOD LookupNamespaceURI(const nsAString & prefix, nsAString & _retval); \
  NS_IMETHOD IsEqualNode(nsIDOMNode *arg, PRBool *_retval); \
  NS_IMETHOD GetFeature(const nsAString & feature, const nsAString & version, nsISupports **_retval); \
  NS_IMETHOD SetUserData(const nsAString & key, nsIVariant *data, nsIDOMUserDataHandler *handler, nsIVariant **_retval); \
  NS_IMETHOD GetUserData(const nsAString & key, nsIVariant **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOM3NODE(_to) \
  NS_IMETHOD GetBaseURI(nsAString & aBaseURI) { return _to GetBaseURI(aBaseURI); } \
  NS_IMETHOD CompareDocumentPosition(nsIDOMNode *other, PRUint16 *_retval) { return _to CompareDocumentPosition(other, _retval); } \
  NS_IMETHOD GetTextContent(nsAString & aTextContent) { return _to GetTextContent(aTextContent); } \
  NS_IMETHOD SetTextContent(const nsAString & aTextContent) { return _to SetTextContent(aTextContent); } \
  NS_IMETHOD IsSameNode(nsIDOMNode *other, PRBool *_retval) { return _to IsSameNode(other, _retval); } \
  NS_IMETHOD LookupPrefix(const nsAString & namespaceURI, nsAString & _retval) { return _to LookupPrefix(namespaceURI, _retval); } \
  NS_IMETHOD IsDefaultNamespace(const nsAString & namespaceURI, PRBool *_retval) { return _to IsDefaultNamespace(namespaceURI, _retval); } \
  NS_IMETHOD LookupNamespaceURI(const nsAString & prefix, nsAString & _retval) { return _to LookupNamespaceURI(prefix, _retval); } \
  NS_IMETHOD IsEqualNode(nsIDOMNode *arg, PRBool *_retval) { return _to IsEqualNode(arg, _retval); } \
  NS_IMETHOD GetFeature(const nsAString & feature, const nsAString & version, nsISupports **_retval) { return _to GetFeature(feature, version, _retval); } \
  NS_IMETHOD SetUserData(const nsAString & key, nsIVariant *data, nsIDOMUserDataHandler *handler, nsIVariant **_retval) { return _to SetUserData(key, data, handler, _retval); } \
  NS_IMETHOD GetUserData(const nsAString & key, nsIVariant **_retval) { return _to GetUserData(key, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOM3NODE(_to) \
  NS_IMETHOD GetBaseURI(nsAString & aBaseURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBaseURI(aBaseURI); } \
  NS_IMETHOD CompareDocumentPosition(nsIDOMNode *other, PRUint16 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CompareDocumentPosition(other, _retval); } \
  NS_IMETHOD GetTextContent(nsAString & aTextContent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTextContent(aTextContent); } \
  NS_IMETHOD SetTextContent(const nsAString & aTextContent) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTextContent(aTextContent); } \
  NS_IMETHOD IsSameNode(nsIDOMNode *other, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsSameNode(other, _retval); } \
  NS_IMETHOD LookupPrefix(const nsAString & namespaceURI, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->LookupPrefix(namespaceURI, _retval); } \
  NS_IMETHOD IsDefaultNamespace(const nsAString & namespaceURI, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsDefaultNamespace(namespaceURI, _retval); } \
  NS_IMETHOD LookupNamespaceURI(const nsAString & prefix, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->LookupNamespaceURI(prefix, _retval); } \
  NS_IMETHOD IsEqualNode(nsIDOMNode *arg, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsEqualNode(arg, _retval); } \
  NS_IMETHOD GetFeature(const nsAString & feature, const nsAString & version, nsISupports **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFeature(feature, version, _retval); } \
  NS_IMETHOD SetUserData(const nsAString & key, nsIVariant *data, nsIDOMUserDataHandler *handler, nsIVariant **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetUserData(key, data, handler, _retval); } \
  NS_IMETHOD GetUserData(const nsAString & key, nsIVariant **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetUserData(key, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOM3Node : public nsIDOM3Node
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOM3NODE

  nsDOM3Node();

private:
  ~nsDOM3Node();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOM3Node, nsIDOM3Node)

nsDOM3Node::nsDOM3Node()
{
  /* member initializers and constructor code */
}

nsDOM3Node::~nsDOM3Node()
{
  /* destructor code */
}

/* readonly attribute DOMString baseURI; */
NS_IMETHODIMP nsDOM3Node::GetBaseURI(nsAString & aBaseURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* unsigned short compareDocumentPosition (in nsIDOMNode other)  raises (DOMException); */
NS_IMETHODIMP nsDOM3Node::CompareDocumentPosition(nsIDOMNode *other, PRUint16 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString textContent; */
NS_IMETHODIMP nsDOM3Node::GetTextContent(nsAString & aTextContent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOM3Node::SetTextContent(const nsAString & aTextContent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isSameNode (in nsIDOMNode other); */
NS_IMETHODIMP nsDOM3Node::IsSameNode(nsIDOMNode *other, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* DOMString lookupPrefix (in DOMString namespaceURI); */
NS_IMETHODIMP nsDOM3Node::LookupPrefix(const nsAString & namespaceURI, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isDefaultNamespace (in DOMString namespaceURI); */
NS_IMETHODIMP nsDOM3Node::IsDefaultNamespace(const nsAString & namespaceURI, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* DOMString lookupNamespaceURI (in DOMString prefix); */
NS_IMETHODIMP nsDOM3Node::LookupNamespaceURI(const nsAString & prefix, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isEqualNode (in nsIDOMNode arg); */
NS_IMETHODIMP nsDOM3Node::IsEqualNode(nsIDOMNode *arg, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsISupports getFeature (in DOMString feature, in DOMString version); */
NS_IMETHODIMP nsDOM3Node::GetFeature(const nsAString & feature, const nsAString & version, nsISupports **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant setUserData (in DOMString key, in nsIVariant data, in nsIDOMUserDataHandler handler); */
NS_IMETHODIMP nsDOM3Node::SetUserData(const nsAString & key, nsIVariant *data, nsIDOMUserDataHandler *handler, nsIVariant **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant getUserData (in DOMString key); */
NS_IMETHODIMP nsDOM3Node::GetUserData(const nsAString & key, nsIVariant **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMDOMConfiguration; /* forward declaration */


/* starting interface:    nsIDOM3Document */
#define NS_IDOM3DOCUMENT_IID_STR "2e0e9ea1-72ab-4d9e-bdeb-ca64e1abeba4"

#define NS_IDOM3DOCUMENT_IID \
  {0x2e0e9ea1, 0x72ab, 0x4d9e, \
    { 0xbd, 0xeb, 0xca, 0x64, 0xe1, 0xab, 0xeb, 0xa4 }}

class NS_NO_VTABLE nsIDOM3Document : public nsIDOM3Node {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOM3DOCUMENT_IID)

  /* readonly attribute DOMString inputEncoding; */
  NS_IMETHOD GetInputEncoding(nsAString & aInputEncoding) = 0;

  /* readonly attribute DOMString xmlEncoding; */
  NS_IMETHOD GetXmlEncoding(nsAString & aXmlEncoding) = 0;

  /* attribute boolean xmlStandalone; */
  NS_IMETHOD GetXmlStandalone(PRBool *aXmlStandalone) = 0;
  NS_IMETHOD SetXmlStandalone(PRBool aXmlStandalone) = 0;

  /* attribute DOMString xmlVersion; */
  NS_IMETHOD GetXmlVersion(nsAString & aXmlVersion) = 0;
  NS_IMETHOD SetXmlVersion(const nsAString & aXmlVersion) = 0;

  /* attribute boolean strictErrorChecking; */
  NS_IMETHOD GetStrictErrorChecking(PRBool *aStrictErrorChecking) = 0;
  NS_IMETHOD SetStrictErrorChecking(PRBool aStrictErrorChecking) = 0;

  /* attribute DOMString documentURI; */
  NS_IMETHOD GetDocumentURI(nsAString & aDocumentURI) = 0;
  NS_IMETHOD SetDocumentURI(const nsAString & aDocumentURI) = 0;

  /* nsIDOMNode adoptNode (in nsIDOMNode source)  raises (DOMException); */
  NS_IMETHOD AdoptNode(nsIDOMNode *source, nsIDOMNode **_retval) = 0;

  /* readonly attribute nsIDOMDOMConfiguration domConfig; */
  NS_IMETHOD GetDomConfig(nsIDOMDOMConfiguration * *aDomConfig) = 0;

  /* void normalizeDocument (); */
  NS_IMETHOD NormalizeDocument(void) = 0;

  /* nsIDOMNode renameNode (in nsIDOMNode node, in DOMString namespaceURI, in DOMString qualifiedName)  raises (DOMException); */
  NS_IMETHOD RenameNode(nsIDOMNode *node, const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMNode **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOM3DOCUMENT \
  NS_IMETHOD GetInputEncoding(nsAString & aInputEncoding); \
  NS_IMETHOD GetXmlEncoding(nsAString & aXmlEncoding); \
  NS_IMETHOD GetXmlStandalone(PRBool *aXmlStandalone); \
  NS_IMETHOD SetXmlStandalone(PRBool aXmlStandalone); \
  NS_IMETHOD GetXmlVersion(nsAString & aXmlVersion); \
  NS_IMETHOD SetXmlVersion(const nsAString & aXmlVersion); \
  NS_IMETHOD GetStrictErrorChecking(PRBool *aStrictErrorChecking); \
  NS_IMETHOD SetStrictErrorChecking(PRBool aStrictErrorChecking); \
  NS_IMETHOD GetDocumentURI(nsAString & aDocumentURI); \
  NS_IMETHOD SetDocumentURI(const nsAString & aDocumentURI); \
  NS_IMETHOD AdoptNode(nsIDOMNode *source, nsIDOMNode **_retval); \
  NS_IMETHOD GetDomConfig(nsIDOMDOMConfiguration * *aDomConfig); \
  NS_IMETHOD NormalizeDocument(void); \
  NS_IMETHOD RenameNode(nsIDOMNode *node, const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMNode **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOM3DOCUMENT(_to) \
  NS_IMETHOD GetInputEncoding(nsAString & aInputEncoding) { return _to GetInputEncoding(aInputEncoding); } \
  NS_IMETHOD GetXmlEncoding(nsAString & aXmlEncoding) { return _to GetXmlEncoding(aXmlEncoding); } \
  NS_IMETHOD GetXmlStandalone(PRBool *aXmlStandalone) { return _to GetXmlStandalone(aXmlStandalone); } \
  NS_IMETHOD SetXmlStandalone(PRBool aXmlStandalone) { return _to SetXmlStandalone(aXmlStandalone); } \
  NS_IMETHOD GetXmlVersion(nsAString & aXmlVersion) { return _to GetXmlVersion(aXmlVersion); } \
  NS_IMETHOD SetXmlVersion(const nsAString & aXmlVersion) { return _to SetXmlVersion(aXmlVersion); } \
  NS_IMETHOD GetStrictErrorChecking(PRBool *aStrictErrorChecking) { return _to GetStrictErrorChecking(aStrictErrorChecking); } \
  NS_IMETHOD SetStrictErrorChecking(PRBool aStrictErrorChecking) { return _to SetStrictErrorChecking(aStrictErrorChecking); } \
  NS_IMETHOD GetDocumentURI(nsAString & aDocumentURI) { return _to GetDocumentURI(aDocumentURI); } \
  NS_IMETHOD SetDocumentURI(const nsAString & aDocumentURI) { return _to SetDocumentURI(aDocumentURI); } \
  NS_IMETHOD AdoptNode(nsIDOMNode *source, nsIDOMNode **_retval) { return _to AdoptNode(source, _retval); } \
  NS_IMETHOD GetDomConfig(nsIDOMDOMConfiguration * *aDomConfig) { return _to GetDomConfig(aDomConfig); } \
  NS_IMETHOD NormalizeDocument(void) { return _to NormalizeDocument(); } \
  NS_IMETHOD RenameNode(nsIDOMNode *node, const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMNode **_retval) { return _to RenameNode(node, namespaceURI, qualifiedName, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOM3DOCUMENT(_to) \
  NS_IMETHOD GetInputEncoding(nsAString & aInputEncoding) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetInputEncoding(aInputEncoding); } \
  NS_IMETHOD GetXmlEncoding(nsAString & aXmlEncoding) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetXmlEncoding(aXmlEncoding); } \
  NS_IMETHOD GetXmlStandalone(PRBool *aXmlStandalone) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetXmlStandalone(aXmlStandalone); } \
  NS_IMETHOD SetXmlStandalone(PRBool aXmlStandalone) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetXmlStandalone(aXmlStandalone); } \
  NS_IMETHOD GetXmlVersion(nsAString & aXmlVersion) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetXmlVersion(aXmlVersion); } \
  NS_IMETHOD SetXmlVersion(const nsAString & aXmlVersion) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetXmlVersion(aXmlVersion); } \
  NS_IMETHOD GetStrictErrorChecking(PRBool *aStrictErrorChecking) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetStrictErrorChecking(aStrictErrorChecking); } \
  NS_IMETHOD SetStrictErrorChecking(PRBool aStrictErrorChecking) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetStrictErrorChecking(aStrictErrorChecking); } \
  NS_IMETHOD GetDocumentURI(nsAString & aDocumentURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDocumentURI(aDocumentURI); } \
  NS_IMETHOD SetDocumentURI(const nsAString & aDocumentURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDocumentURI(aDocumentURI); } \
  NS_IMETHOD AdoptNode(nsIDOMNode *source, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->AdoptNode(source, _retval); } \
  NS_IMETHOD GetDomConfig(nsIDOMDOMConfiguration * *aDomConfig) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDomConfig(aDomConfig); } \
  NS_IMETHOD NormalizeDocument(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->NormalizeDocument(); } \
  NS_IMETHOD RenameNode(nsIDOMNode *node, const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->RenameNode(node, namespaceURI, qualifiedName, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOM3Document : public nsIDOM3Document
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOM3DOCUMENT

  nsDOM3Document();

private:
  ~nsDOM3Document();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOM3Document, nsIDOM3Document)

nsDOM3Document::nsDOM3Document()
{
  /* member initializers and constructor code */
}

nsDOM3Document::~nsDOM3Document()
{
  /* destructor code */
}

/* readonly attribute DOMString inputEncoding; */
NS_IMETHODIMP nsDOM3Document::GetInputEncoding(nsAString & aInputEncoding)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute DOMString xmlEncoding; */
NS_IMETHODIMP nsDOM3Document::GetXmlEncoding(nsAString & aXmlEncoding)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean xmlStandalone; */
NS_IMETHODIMP nsDOM3Document::GetXmlStandalone(PRBool *aXmlStandalone)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOM3Document::SetXmlStandalone(PRBool aXmlStandalone)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString xmlVersion; */
NS_IMETHODIMP nsDOM3Document::GetXmlVersion(nsAString & aXmlVersion)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOM3Document::SetXmlVersion(const nsAString & aXmlVersion)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean strictErrorChecking; */
NS_IMETHODIMP nsDOM3Document::GetStrictErrorChecking(PRBool *aStrictErrorChecking)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOM3Document::SetStrictErrorChecking(PRBool aStrictErrorChecking)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString documentURI; */
NS_IMETHODIMP nsDOM3Document::GetDocumentURI(nsAString & aDocumentURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOM3Document::SetDocumentURI(const nsAString & aDocumentURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode adoptNode (in nsIDOMNode source)  raises (DOMException); */
NS_IMETHODIMP nsDOM3Document::AdoptNode(nsIDOMNode *source, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMDOMConfiguration domConfig; */
NS_IMETHODIMP nsDOM3Document::GetDomConfig(nsIDOMDOMConfiguration * *aDomConfig)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void normalizeDocument (); */
NS_IMETHODIMP nsDOM3Document::NormalizeDocument()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode renameNode (in nsIDOMNode node, in DOMString namespaceURI, in DOMString qualifiedName)  raises (DOMException); */
NS_IMETHODIMP nsDOM3Document::RenameNode(nsIDOMNode *node, const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMElement; /* forward declaration */


/* starting interface:    nsIDOMAttr */
#define NS_IDOMATTR_IID_STR "a6cf9070-15b3-11d2-932e-00805f8add32"

#define NS_IDOMATTR_IID \
  {0xa6cf9070, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMAttr : public nsIDOMNode {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMATTR_IID)

  /* readonly attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;

  /* readonly attribute boolean specified; */
  NS_IMETHOD GetSpecified(PRBool *aSpecified) = 0;

  /* attribute DOMString value; */
  NS_IMETHOD GetValue(nsAString & aValue) = 0;
  NS_IMETHOD SetValue(const nsAString & aValue) = 0;

  /* readonly attribute nsIDOMElement ownerElement; */
  NS_IMETHOD GetOwnerElement(nsIDOMElement * *aOwnerElement) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMATTR \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD GetSpecified(PRBool *aSpecified); \
  NS_IMETHOD GetValue(nsAString & aValue); \
  NS_IMETHOD SetValue(const nsAString & aValue); \
  NS_IMETHOD GetOwnerElement(nsIDOMElement * *aOwnerElement); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMATTR(_to) \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD GetSpecified(PRBool *aSpecified) { return _to GetSpecified(aSpecified); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return _to GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return _to SetValue(aValue); } \
  NS_IMETHOD GetOwnerElement(nsIDOMElement * *aOwnerElement) { return _to GetOwnerElement(aOwnerElement); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMATTR(_to) \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD GetSpecified(PRBool *aSpecified) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSpecified(aSpecified); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValue(aValue); } \
  NS_IMETHOD GetOwnerElement(nsIDOMElement * *aOwnerElement) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOwnerElement(aOwnerElement); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMAttr : public nsIDOMAttr
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMATTR

  nsDOMAttr();

private:
  ~nsDOMAttr();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMAttr, nsIDOMAttr)

nsDOMAttr::nsDOMAttr()
{
  /* member initializers and constructor code */
}

nsDOMAttr::~nsDOMAttr()
{
  /* destructor code */
}

/* readonly attribute DOMString name; */
NS_IMETHODIMP nsDOMAttr::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean specified; */
NS_IMETHODIMP nsDOMAttr::GetSpecified(PRBool *aSpecified)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString value; */
NS_IMETHODIMP nsDOMAttr::GetValue(nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMAttr::SetValue(const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMElement ownerElement; */
NS_IMETHODIMP nsDOMAttr::GetOwnerElement(nsIDOMElement * *aOwnerElement)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMAttr; /* forward declaration */

class DOMException; /* forward declaration */


/* starting interface:    nsIDOMElement */
#define NS_IDOMELEMENT_IID_STR "a6cf9078-15b3-11d2-932e-00805f8add32"

#define NS_IDOMELEMENT_IID \
  {0xa6cf9078, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMElement : public nsIDOMNode {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMELEMENT_IID)

  /* readonly attribute DOMString tagName; */
  NS_IMETHOD GetTagName(nsAString & aTagName) = 0;

  /* DOMString getAttribute (in DOMString name); */
  NS_IMETHOD GetAttribute(const nsAString & name, nsAString & _retval) = 0;

  /* void setAttribute (in DOMString name, in DOMString value)  raises (DOMException); */
  NS_IMETHOD SetAttribute(const nsAString & name, const nsAString & value) = 0;

  /* void removeAttribute (in DOMString name)  raises (DOMException); */
  NS_IMETHOD RemoveAttribute(const nsAString & name) = 0;

  /* nsIDOMAttr getAttributeNode (in DOMString name); */
  NS_IMETHOD GetAttributeNode(const nsAString & name, nsIDOMAttr **_retval) = 0;

  /* nsIDOMAttr setAttributeNode (in nsIDOMAttr new_attr)  raises (DOMException); */
  NS_IMETHOD SetAttributeNode(nsIDOMAttr *new_attr, nsIDOMAttr **_retval) = 0;

  /* nsIDOMAttr removeAttributeNode (in nsIDOMAttr old_attr)  raises (DOMException); */
  NS_IMETHOD RemoveAttributeNode(nsIDOMAttr *old_attr, nsIDOMAttr **_retval) = 0;

  /* nsIDOMNodeList getElementsByTagName (in DOMString name); */
  NS_IMETHOD GetElementsByTagName(const nsAString & name, nsIDOMNodeList **_retval) = 0;

  /* DOMString getAttributeNS (in DOMString namespace_uri, in DOMString local_name); */
  NS_IMETHOD GetAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, nsAString & _retval) = 0;

  /* void setAttributeNS (in DOMString namespace_uri, in DOMString qualified_name, in DOMString value)  raises (DOMException); */
  NS_IMETHOD SetAttributeNS(const nsAString & namespace_uri, const nsAString & qualified_name, const nsAString & value) = 0;

  /* void removeAttributeNS (in DOMString namespace_uri, in DOMString local_name)  raises (DOMException); */
  NS_IMETHOD RemoveAttributeNS(const nsAString & namespace_uri, const nsAString & local_name) = 0;

  /* nsIDOMAttr getAttributeNodeNS (in DOMString namespace_uri, in DOMString local_name); */
  NS_IMETHOD GetAttributeNodeNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMAttr **_retval) = 0;

  /* nsIDOMAttr setAttributeNodeNS (in nsIDOMAttr newAttr)  raises (DOMException); */
  NS_IMETHOD SetAttributeNodeNS(nsIDOMAttr *newAttr, nsIDOMAttr **_retval) = 0;

  /* nsIDOMNodeList getElementsByTagNameNS (in DOMString namespace_uri, in DOMString local_name); */
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNodeList **_retval) = 0;

  /* boolean hasAttribute (in DOMString name); */
  NS_IMETHOD HasAttribute(const nsAString & name, PRBool *_retval) = 0;

  /* boolean hasAttributeNS (in DOMString namespace_uri, in DOMString local_name); */
  NS_IMETHOD HasAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMELEMENT \
  NS_IMETHOD GetTagName(nsAString & aTagName); \
  NS_IMETHOD GetAttribute(const nsAString & name, nsAString & _retval); \
  NS_IMETHOD SetAttribute(const nsAString & name, const nsAString & value); \
  NS_IMETHOD RemoveAttribute(const nsAString & name); \
  NS_IMETHOD GetAttributeNode(const nsAString & name, nsIDOMAttr **_retval); \
  NS_IMETHOD SetAttributeNode(nsIDOMAttr *new_attr, nsIDOMAttr **_retval); \
  NS_IMETHOD RemoveAttributeNode(nsIDOMAttr *old_attr, nsIDOMAttr **_retval); \
  NS_IMETHOD GetElementsByTagName(const nsAString & name, nsIDOMNodeList **_retval); \
  NS_IMETHOD GetAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, nsAString & _retval); \
  NS_IMETHOD SetAttributeNS(const nsAString & namespace_uri, const nsAString & qualified_name, const nsAString & value); \
  NS_IMETHOD RemoveAttributeNS(const nsAString & namespace_uri, const nsAString & local_name); \
  NS_IMETHOD GetAttributeNodeNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMAttr **_retval); \
  NS_IMETHOD SetAttributeNodeNS(nsIDOMAttr *newAttr, nsIDOMAttr **_retval); \
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNodeList **_retval); \
  NS_IMETHOD HasAttribute(const nsAString & name, PRBool *_retval); \
  NS_IMETHOD HasAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMELEMENT(_to) \
  NS_IMETHOD GetTagName(nsAString & aTagName) { return _to GetTagName(aTagName); } \
  NS_IMETHOD GetAttribute(const nsAString & name, nsAString & _retval) { return _to GetAttribute(name, _retval); } \
  NS_IMETHOD SetAttribute(const nsAString & name, const nsAString & value) { return _to SetAttribute(name, value); } \
  NS_IMETHOD RemoveAttribute(const nsAString & name) { return _to RemoveAttribute(name); } \
  NS_IMETHOD GetAttributeNode(const nsAString & name, nsIDOMAttr **_retval) { return _to GetAttributeNode(name, _retval); } \
  NS_IMETHOD SetAttributeNode(nsIDOMAttr *new_attr, nsIDOMAttr **_retval) { return _to SetAttributeNode(new_attr, _retval); } \
  NS_IMETHOD RemoveAttributeNode(nsIDOMAttr *old_attr, nsIDOMAttr **_retval) { return _to RemoveAttributeNode(old_attr, _retval); } \
  NS_IMETHOD GetElementsByTagName(const nsAString & name, nsIDOMNodeList **_retval) { return _to GetElementsByTagName(name, _retval); } \
  NS_IMETHOD GetAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, nsAString & _retval) { return _to GetAttributeNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD SetAttributeNS(const nsAString & namespace_uri, const nsAString & qualified_name, const nsAString & value) { return _to SetAttributeNS(namespace_uri, qualified_name, value); } \
  NS_IMETHOD RemoveAttributeNS(const nsAString & namespace_uri, const nsAString & local_name) { return _to RemoveAttributeNS(namespace_uri, local_name); } \
  NS_IMETHOD GetAttributeNodeNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMAttr **_retval) { return _to GetAttributeNodeNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD SetAttributeNodeNS(nsIDOMAttr *newAttr, nsIDOMAttr **_retval) { return _to SetAttributeNodeNS(newAttr, _retval); } \
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNodeList **_retval) { return _to GetElementsByTagNameNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD HasAttribute(const nsAString & name, PRBool *_retval) { return _to HasAttribute(name, _retval); } \
  NS_IMETHOD HasAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, PRBool *_retval) { return _to HasAttributeNS(namespace_uri, local_name, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMELEMENT(_to) \
  NS_IMETHOD GetTagName(nsAString & aTagName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTagName(aTagName); } \
  NS_IMETHOD GetAttribute(const nsAString & name, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAttribute(name, _retval); } \
  NS_IMETHOD SetAttribute(const nsAString & name, const nsAString & value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAttribute(name, value); } \
  NS_IMETHOD RemoveAttribute(const nsAString & name) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveAttribute(name); } \
  NS_IMETHOD GetAttributeNode(const nsAString & name, nsIDOMAttr **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAttributeNode(name, _retval); } \
  NS_IMETHOD SetAttributeNode(nsIDOMAttr *new_attr, nsIDOMAttr **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAttributeNode(new_attr, _retval); } \
  NS_IMETHOD RemoveAttributeNode(nsIDOMAttr *old_attr, nsIDOMAttr **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveAttributeNode(old_attr, _retval); } \
  NS_IMETHOD GetElementsByTagName(const nsAString & name, nsIDOMNodeList **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetElementsByTagName(name, _retval); } \
  NS_IMETHOD GetAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAttributeNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD SetAttributeNS(const nsAString & namespace_uri, const nsAString & qualified_name, const nsAString & value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAttributeNS(namespace_uri, qualified_name, value); } \
  NS_IMETHOD RemoveAttributeNS(const nsAString & namespace_uri, const nsAString & local_name) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveAttributeNS(namespace_uri, local_name); } \
  NS_IMETHOD GetAttributeNodeNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMAttr **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAttributeNodeNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD SetAttributeNodeNS(nsIDOMAttr *newAttr, nsIDOMAttr **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAttributeNodeNS(newAttr, _retval); } \
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNodeList **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetElementsByTagNameNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD HasAttribute(const nsAString & name, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->HasAttribute(name, _retval); } \
  NS_IMETHOD HasAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->HasAttributeNS(namespace_uri, local_name, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMElement : public nsIDOMElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMELEMENT

  nsDOMElement();

private:
  ~nsDOMElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMElement, nsIDOMElement)

nsDOMElement::nsDOMElement()
{
  /* member initializers and constructor code */
}

nsDOMElement::~nsDOMElement()
{
  /* destructor code */
}

/* readonly attribute DOMString tagName; */
NS_IMETHODIMP nsDOMElement::GetTagName(nsAString & aTagName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* DOMString getAttribute (in DOMString name); */
NS_IMETHODIMP nsDOMElement::GetAttribute(const nsAString & name, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setAttribute (in DOMString name, in DOMString value)  raises (DOMException); */
NS_IMETHODIMP nsDOMElement::SetAttribute(const nsAString & name, const nsAString & value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeAttribute (in DOMString name)  raises (DOMException); */
NS_IMETHODIMP nsDOMElement::RemoveAttribute(const nsAString & name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMAttr getAttributeNode (in DOMString name); */
NS_IMETHODIMP nsDOMElement::GetAttributeNode(const nsAString & name, nsIDOMAttr **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMAttr setAttributeNode (in nsIDOMAttr new_attr)  raises (DOMException); */
NS_IMETHODIMP nsDOMElement::SetAttributeNode(nsIDOMAttr *new_attr, nsIDOMAttr **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMAttr removeAttributeNode (in nsIDOMAttr old_attr)  raises (DOMException); */
NS_IMETHODIMP nsDOMElement::RemoveAttributeNode(nsIDOMAttr *old_attr, nsIDOMAttr **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNodeList getElementsByTagName (in DOMString name); */
NS_IMETHODIMP nsDOMElement::GetElementsByTagName(const nsAString & name, nsIDOMNodeList **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* DOMString getAttributeNS (in DOMString namespace_uri, in DOMString local_name); */
NS_IMETHODIMP nsDOMElement::GetAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setAttributeNS (in DOMString namespace_uri, in DOMString qualified_name, in DOMString value)  raises (DOMException); */
NS_IMETHODIMP nsDOMElement::SetAttributeNS(const nsAString & namespace_uri, const nsAString & qualified_name, const nsAString & value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeAttributeNS (in DOMString namespace_uri, in DOMString local_name)  raises (DOMException); */
NS_IMETHODIMP nsDOMElement::RemoveAttributeNS(const nsAString & namespace_uri, const nsAString & local_name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMAttr getAttributeNodeNS (in DOMString namespace_uri, in DOMString local_name); */
NS_IMETHODIMP nsDOMElement::GetAttributeNodeNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMAttr **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMAttr setAttributeNodeNS (in nsIDOMAttr newAttr)  raises (DOMException); */
NS_IMETHODIMP nsDOMElement::SetAttributeNodeNS(nsIDOMAttr *newAttr, nsIDOMAttr **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNodeList getElementsByTagNameNS (in DOMString namespace_uri, in DOMString local_name); */
NS_IMETHODIMP nsDOMElement::GetElementsByTagNameNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNodeList **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean hasAttribute (in DOMString name); */
NS_IMETHODIMP nsDOMElement::HasAttribute(const nsAString & name, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean hasAttributeNS (in DOMString namespace_uri, in DOMString local_name); */
NS_IMETHODIMP nsDOMElement::HasAttributeNS(const nsAString & namespace_uri, const nsAString & local_name, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDOMCharacterData */
#define NS_IDOMCHARACTERDATA_IID_STR "a6cf9072-15b3-11d2-932e-00805f8add32"

#define NS_IDOMCHARACTERDATA_IID \
  {0xa6cf9072, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMCharacterData : public nsIDOMNode {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMCHARACTERDATA_IID)

  /* attribute DOMString data; */
  NS_IMETHOD GetData(nsAString & aData) = 0;
  NS_IMETHOD SetData(const nsAString & aData) = 0;

  /* readonly attribute unsigned long length; */
  NS_IMETHOD GetLength(PRUint32 *aLength) = 0;

  /* DOMString substringData (in unsigned long offset, in unsigned long count)  raises (DOMException); */
  NS_IMETHOD SubstringData(PRUint32 offset, PRUint32 count, nsAString & _retval) = 0;

  /* void appendData (in DOMString arg)  raises (DOMException); */
  NS_IMETHOD AppendData(const nsAString & arg) = 0;

  /* void insertData (in unsigned long offset, in DOMString arg)  raises (DOMException); */
  NS_IMETHOD InsertData(PRUint32 offset, const nsAString & arg) = 0;

  /* void deleteData (in unsigned long offset, in unsigned long count)  raises (DOMException); */
  NS_IMETHOD DeleteData(PRUint32 offset, PRUint32 count) = 0;

  /* void replaceData (in unsigned long offset, in unsigned long count, in DOMString arg)  raises (DOMException); */
  NS_IMETHOD ReplaceData(PRUint32 offset, PRUint32 count, const nsAString & arg) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMCHARACTERDATA \
  NS_IMETHOD GetData(nsAString & aData); \
  NS_IMETHOD SetData(const nsAString & aData); \
  NS_IMETHOD GetLength(PRUint32 *aLength); \
  NS_IMETHOD SubstringData(PRUint32 offset, PRUint32 count, nsAString & _retval); \
  NS_IMETHOD AppendData(const nsAString & arg); \
  NS_IMETHOD InsertData(PRUint32 offset, const nsAString & arg); \
  NS_IMETHOD DeleteData(PRUint32 offset, PRUint32 count); \
  NS_IMETHOD ReplaceData(PRUint32 offset, PRUint32 count, const nsAString & arg); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMCHARACTERDATA(_to) \
  NS_IMETHOD GetData(nsAString & aData) { return _to GetData(aData); } \
  NS_IMETHOD SetData(const nsAString & aData) { return _to SetData(aData); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return _to GetLength(aLength); } \
  NS_IMETHOD SubstringData(PRUint32 offset, PRUint32 count, nsAString & _retval) { return _to SubstringData(offset, count, _retval); } \
  NS_IMETHOD AppendData(const nsAString & arg) { return _to AppendData(arg); } \
  NS_IMETHOD InsertData(PRUint32 offset, const nsAString & arg) { return _to InsertData(offset, arg); } \
  NS_IMETHOD DeleteData(PRUint32 offset, PRUint32 count) { return _to DeleteData(offset, count); } \
  NS_IMETHOD ReplaceData(PRUint32 offset, PRUint32 count, const nsAString & arg) { return _to ReplaceData(offset, count, arg); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMCHARACTERDATA(_to) \
  NS_IMETHOD GetData(nsAString & aData) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetData(aData); } \
  NS_IMETHOD SetData(const nsAString & aData) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetData(aData); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLength(aLength); } \
  NS_IMETHOD SubstringData(PRUint32 offset, PRUint32 count, nsAString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SubstringData(offset, count, _retval); } \
  NS_IMETHOD AppendData(const nsAString & arg) { return !_to ? NS_ERROR_NULL_POINTER : _to->AppendData(arg); } \
  NS_IMETHOD InsertData(PRUint32 offset, const nsAString & arg) { return !_to ? NS_ERROR_NULL_POINTER : _to->InsertData(offset, arg); } \
  NS_IMETHOD DeleteData(PRUint32 offset, PRUint32 count) { return !_to ? NS_ERROR_NULL_POINTER : _to->DeleteData(offset, count); } \
  NS_IMETHOD ReplaceData(PRUint32 offset, PRUint32 count, const nsAString & arg) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReplaceData(offset, count, arg); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMCharacterData : public nsIDOMCharacterData
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMCHARACTERDATA

  nsDOMCharacterData();

private:
  ~nsDOMCharacterData();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMCharacterData, nsIDOMCharacterData)

nsDOMCharacterData::nsDOMCharacterData()
{
  /* member initializers and constructor code */
}

nsDOMCharacterData::~nsDOMCharacterData()
{
  /* destructor code */
}

/* attribute DOMString data; */
NS_IMETHODIMP nsDOMCharacterData::GetData(nsAString & aData)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMCharacterData::SetData(const nsAString & aData)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned long length; */
NS_IMETHODIMP nsDOMCharacterData::GetLength(PRUint32 *aLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* DOMString substringData (in unsigned long offset, in unsigned long count)  raises (DOMException); */
NS_IMETHODIMP nsDOMCharacterData::SubstringData(PRUint32 offset, PRUint32 count, nsAString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void appendData (in DOMString arg)  raises (DOMException); */
NS_IMETHODIMP nsDOMCharacterData::AppendData(const nsAString & arg)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void insertData (in unsigned long offset, in DOMString arg)  raises (DOMException); */
NS_IMETHODIMP nsDOMCharacterData::InsertData(PRUint32 offset, const nsAString & arg)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void deleteData (in unsigned long offset, in unsigned long count)  raises (DOMException); */
NS_IMETHODIMP nsDOMCharacterData::DeleteData(PRUint32 offset, PRUint32 count)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void replaceData (in unsigned long offset, in unsigned long count, in DOMString arg)  raises (DOMException); */
NS_IMETHODIMP nsDOMCharacterData::ReplaceData(PRUint32 offset, PRUint32 count, const nsAString & arg)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMText; /* forward declaration */

class nsIDOMDocumentType; /* forward declaration */

class nsIDOMDOMImplementation; /* forward declaration */

class nsIDOMElement; /* forward declaration */

class nsIDOMDocumentFragment; /* forward declaration */

class nsIDOMComment; /* forward declaration */

class nsIDOMCDATASection; /* forward declaration */

class nsIDOMProcessingInstruction; /* forward declaration */

class nsIDOMAttr; /* forward declaration */

class nsIDOMEntityReference; /* forward declaration */


/* starting interface:    nsIDOMDocument */
#define NS_IDOMDOCUMENT_IID_STR "a6cf9075-15b3-11d2-932e-00805f8add32"

#define NS_IDOMDOCUMENT_IID \
  {0xa6cf9075, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMDocument : public nsIDOMNode {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMDOCUMENT_IID)

  /* readonly attribute nsIDOMDocumentType doctype; */
  NS_IMETHOD GetDoctype(nsIDOMDocumentType * *aDoctype) = 0;

  /* readonly attribute nsIDOMDOMImplementation implementation; */
  NS_IMETHOD GetImplementation(nsIDOMDOMImplementation * *aImplementation) = 0;

  /* readonly attribute nsIDOMElement documentElement; */
  NS_IMETHOD GetDocumentElement(nsIDOMElement * *aDocumentElement) = 0;

  /* nsIDOMElement createElement (in DOMString tagName)  raises (DOMException); */
  NS_IMETHOD CreateElement(const nsAString & tagName, nsIDOMElement **_retval) = 0;

  /* nsIDOMDocumentFragment createDocumentFragment (); */
  NS_IMETHOD CreateDocumentFragment(nsIDOMDocumentFragment **_retval) = 0;

  /* nsIDOMText createTextNode (in DOMString data); */
  NS_IMETHOD CreateTextNode(const nsAString & data, nsIDOMText **_retval) = 0;

  /* nsIDOMComment createComment (in DOMString data); */
  NS_IMETHOD CreateComment(const nsAString & data, nsIDOMComment **_retval) = 0;

  /* nsIDOMCDATASection createCDATASection (in DOMString data)  raises (DOMException); */
  NS_IMETHOD CreateCDATASection(const nsAString & data, nsIDOMCDATASection **_retval) = 0;

  /* nsIDOMProcessingInstruction createProcessingInstruction (in DOMString target, in DOMString data)  raises (DOMException); */
  NS_IMETHOD CreateProcessingInstruction(const nsAString & target, const nsAString & data, nsIDOMProcessingInstruction **_retval) = 0;

  /* nsIDOMAttr createAttribute (in DOMString name)  raises (DOMException); */
  NS_IMETHOD CreateAttribute(const nsAString & name, nsIDOMAttr **_retval) = 0;

  /* nsIDOMEntityReference createEntityReference (in DOMString name)  raises (DOMException); */
  NS_IMETHOD CreateEntityReference(const nsAString & name, nsIDOMEntityReference **_retval) = 0;

  /* nsIDOMNodeList getElementsByTagName (in DOMString tagname); */
  NS_IMETHOD GetElementsByTagName(const nsAString & tagname, nsIDOMNodeList **_retval) = 0;

  /* nsIDOMNode importNode (in nsIDOMNode importedNode, in boolean deep)  raises (DOMException); */
  NS_IMETHOD ImportNode(nsIDOMNode *importedNode, PRBool deep, nsIDOMNode **_retval) = 0;

  /* nsIDOMElement createElementNS (in DOMString namespaceURI, in DOMString qualifiedName)  raises (DOMException); */
  NS_IMETHOD CreateElementNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMElement **_retval) = 0;

  /* nsIDOMAttr createAttributeNS (in DOMString namespaceURI, in DOMString qualifiedName)  raises (DOMException); */
  NS_IMETHOD CreateAttributeNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMAttr **_retval) = 0;

  /* nsIDOMNodeList getElementsByTagNameNS (in DOMString namespaceURI, in DOMString localName); */
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespaceURI, const nsAString & localName, nsIDOMNodeList **_retval) = 0;

  /* nsIDOMElement getElementById (in DOMString elementId); */
  NS_IMETHOD GetElementById(const nsAString & elementId, nsIDOMElement **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMDOCUMENT \
  NS_IMETHOD GetDoctype(nsIDOMDocumentType * *aDoctype); \
  NS_IMETHOD GetImplementation(nsIDOMDOMImplementation * *aImplementation); \
  NS_IMETHOD GetDocumentElement(nsIDOMElement * *aDocumentElement); \
  NS_IMETHOD CreateElement(const nsAString & tagName, nsIDOMElement **_retval); \
  NS_IMETHOD CreateDocumentFragment(nsIDOMDocumentFragment **_retval); \
  NS_IMETHOD CreateTextNode(const nsAString & data, nsIDOMText **_retval); \
  NS_IMETHOD CreateComment(const nsAString & data, nsIDOMComment **_retval); \
  NS_IMETHOD CreateCDATASection(const nsAString & data, nsIDOMCDATASection **_retval); \
  NS_IMETHOD CreateProcessingInstruction(const nsAString & target, const nsAString & data, nsIDOMProcessingInstruction **_retval); \
  NS_IMETHOD CreateAttribute(const nsAString & name, nsIDOMAttr **_retval); \
  NS_IMETHOD CreateEntityReference(const nsAString & name, nsIDOMEntityReference **_retval); \
  NS_IMETHOD GetElementsByTagName(const nsAString & tagname, nsIDOMNodeList **_retval); \
  NS_IMETHOD ImportNode(nsIDOMNode *importedNode, PRBool deep, nsIDOMNode **_retval); \
  NS_IMETHOD CreateElementNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMElement **_retval); \
  NS_IMETHOD CreateAttributeNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMAttr **_retval); \
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespaceURI, const nsAString & localName, nsIDOMNodeList **_retval); \
  NS_IMETHOD GetElementById(const nsAString & elementId, nsIDOMElement **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMDOCUMENT(_to) \
  NS_IMETHOD GetDoctype(nsIDOMDocumentType * *aDoctype) { return _to GetDoctype(aDoctype); } \
  NS_IMETHOD GetImplementation(nsIDOMDOMImplementation * *aImplementation) { return _to GetImplementation(aImplementation); } \
  NS_IMETHOD GetDocumentElement(nsIDOMElement * *aDocumentElement) { return _to GetDocumentElement(aDocumentElement); } \
  NS_IMETHOD CreateElement(const nsAString & tagName, nsIDOMElement **_retval) { return _to CreateElement(tagName, _retval); } \
  NS_IMETHOD CreateDocumentFragment(nsIDOMDocumentFragment **_retval) { return _to CreateDocumentFragment(_retval); } \
  NS_IMETHOD CreateTextNode(const nsAString & data, nsIDOMText **_retval) { return _to CreateTextNode(data, _retval); } \
  NS_IMETHOD CreateComment(const nsAString & data, nsIDOMComment **_retval) { return _to CreateComment(data, _retval); } \
  NS_IMETHOD CreateCDATASection(const nsAString & data, nsIDOMCDATASection **_retval) { return _to CreateCDATASection(data, _retval); } \
  NS_IMETHOD CreateProcessingInstruction(const nsAString & target, const nsAString & data, nsIDOMProcessingInstruction **_retval) { return _to CreateProcessingInstruction(target, data, _retval); } \
  NS_IMETHOD CreateAttribute(const nsAString & name, nsIDOMAttr **_retval) { return _to CreateAttribute(name, _retval); } \
  NS_IMETHOD CreateEntityReference(const nsAString & name, nsIDOMEntityReference **_retval) { return _to CreateEntityReference(name, _retval); } \
  NS_IMETHOD GetElementsByTagName(const nsAString & tagname, nsIDOMNodeList **_retval) { return _to GetElementsByTagName(tagname, _retval); } \
  NS_IMETHOD ImportNode(nsIDOMNode *importedNode, PRBool deep, nsIDOMNode **_retval) { return _to ImportNode(importedNode, deep, _retval); } \
  NS_IMETHOD CreateElementNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMElement **_retval) { return _to CreateElementNS(namespaceURI, qualifiedName, _retval); } \
  NS_IMETHOD CreateAttributeNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMAttr **_retval) { return _to CreateAttributeNS(namespaceURI, qualifiedName, _retval); } \
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespaceURI, const nsAString & localName, nsIDOMNodeList **_retval) { return _to GetElementsByTagNameNS(namespaceURI, localName, _retval); } \
  NS_IMETHOD GetElementById(const nsAString & elementId, nsIDOMElement **_retval) { return _to GetElementById(elementId, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMDOCUMENT(_to) \
  NS_IMETHOD GetDoctype(nsIDOMDocumentType * *aDoctype) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDoctype(aDoctype); } \
  NS_IMETHOD GetImplementation(nsIDOMDOMImplementation * *aImplementation) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetImplementation(aImplementation); } \
  NS_IMETHOD GetDocumentElement(nsIDOMElement * *aDocumentElement) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDocumentElement(aDocumentElement); } \
  NS_IMETHOD CreateElement(const nsAString & tagName, nsIDOMElement **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateElement(tagName, _retval); } \
  NS_IMETHOD CreateDocumentFragment(nsIDOMDocumentFragment **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateDocumentFragment(_retval); } \
  NS_IMETHOD CreateTextNode(const nsAString & data, nsIDOMText **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateTextNode(data, _retval); } \
  NS_IMETHOD CreateComment(const nsAString & data, nsIDOMComment **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateComment(data, _retval); } \
  NS_IMETHOD CreateCDATASection(const nsAString & data, nsIDOMCDATASection **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateCDATASection(data, _retval); } \
  NS_IMETHOD CreateProcessingInstruction(const nsAString & target, const nsAString & data, nsIDOMProcessingInstruction **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateProcessingInstruction(target, data, _retval); } \
  NS_IMETHOD CreateAttribute(const nsAString & name, nsIDOMAttr **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateAttribute(name, _retval); } \
  NS_IMETHOD CreateEntityReference(const nsAString & name, nsIDOMEntityReference **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateEntityReference(name, _retval); } \
  NS_IMETHOD GetElementsByTagName(const nsAString & tagname, nsIDOMNodeList **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetElementsByTagName(tagname, _retval); } \
  NS_IMETHOD ImportNode(nsIDOMNode *importedNode, PRBool deep, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ImportNode(importedNode, deep, _retval); } \
  NS_IMETHOD CreateElementNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMElement **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateElementNS(namespaceURI, qualifiedName, _retval); } \
  NS_IMETHOD CreateAttributeNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMAttr **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateAttributeNS(namespaceURI, qualifiedName, _retval); } \
  NS_IMETHOD GetElementsByTagNameNS(const nsAString & namespaceURI, const nsAString & localName, nsIDOMNodeList **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetElementsByTagNameNS(namespaceURI, localName, _retval); } \
  NS_IMETHOD GetElementById(const nsAString & elementId, nsIDOMElement **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetElementById(elementId, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMDocument : public nsIDOMDocument
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMDOCUMENT

  nsDOMDocument();

private:
  ~nsDOMDocument();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMDocument, nsIDOMDocument)

nsDOMDocument::nsDOMDocument()
{
  /* member initializers and constructor code */
}

nsDOMDocument::~nsDOMDocument()
{
  /* destructor code */
}

/* readonly attribute nsIDOMDocumentType doctype; */
NS_IMETHODIMP nsDOMDocument::GetDoctype(nsIDOMDocumentType * *aDoctype)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMDOMImplementation implementation; */
NS_IMETHODIMP nsDOMDocument::GetImplementation(nsIDOMDOMImplementation * *aImplementation)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMElement documentElement; */
NS_IMETHODIMP nsDOMDocument::GetDocumentElement(nsIDOMElement * *aDocumentElement)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMElement createElement (in DOMString tagName)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::CreateElement(const nsAString & tagName, nsIDOMElement **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMDocumentFragment createDocumentFragment (); */
NS_IMETHODIMP nsDOMDocument::CreateDocumentFragment(nsIDOMDocumentFragment **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMText createTextNode (in DOMString data); */
NS_IMETHODIMP nsDOMDocument::CreateTextNode(const nsAString & data, nsIDOMText **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMComment createComment (in DOMString data); */
NS_IMETHODIMP nsDOMDocument::CreateComment(const nsAString & data, nsIDOMComment **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMCDATASection createCDATASection (in DOMString data)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::CreateCDATASection(const nsAString & data, nsIDOMCDATASection **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMProcessingInstruction createProcessingInstruction (in DOMString target, in DOMString data)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::CreateProcessingInstruction(const nsAString & target, const nsAString & data, nsIDOMProcessingInstruction **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMAttr createAttribute (in DOMString name)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::CreateAttribute(const nsAString & name, nsIDOMAttr **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMEntityReference createEntityReference (in DOMString name)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::CreateEntityReference(const nsAString & name, nsIDOMEntityReference **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNodeList getElementsByTagName (in DOMString tagname); */
NS_IMETHODIMP nsDOMDocument::GetElementsByTagName(const nsAString & tagname, nsIDOMNodeList **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode importNode (in nsIDOMNode importedNode, in boolean deep)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::ImportNode(nsIDOMNode *importedNode, PRBool deep, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMElement createElementNS (in DOMString namespaceURI, in DOMString qualifiedName)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::CreateElementNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMElement **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMAttr createAttributeNS (in DOMString namespaceURI, in DOMString qualifiedName)  raises (DOMException); */
NS_IMETHODIMP nsDOMDocument::CreateAttributeNS(const nsAString & namespaceURI, const nsAString & qualifiedName, nsIDOMAttr **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNodeList getElementsByTagNameNS (in DOMString namespaceURI, in DOMString localName); */
NS_IMETHODIMP nsDOMDocument::GetElementsByTagNameNS(const nsAString & namespaceURI, const nsAString & localName, nsIDOMNodeList **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMElement getElementById (in DOMString elementId); */
NS_IMETHODIMP nsDOMDocument::GetElementById(const nsAString & elementId, nsIDOMElement **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMAbstractView; /* forward declaration */

class nsIDOMEventTarget; /* forward declaration */

class DOMTimeStamp; /* forward declaration */


/* starting interface:    nsIDOMEvent */
#define NS_IDOMEVENT_IID_STR "a66b7b80-ff46-bd97-0080-5f8ae38add32"

#define NS_IDOMEVENT_IID \
  {0xa66b7b80, 0xff46, 0xbd97, \
    { 0x00, 0x80, 0x5f, 0x8a, 0xe3, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMEvent : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMEVENT_IID)

  enum { CAPTURING_PHASE = 1U };

  enum { AT_TARGET = 2U };

  enum { BUBBLING_PHASE = 3U };

  /* readonly attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;

  /* readonly attribute nsIDOMEventTarget target; */
  NS_IMETHOD GetTarget(nsIDOMEventTarget * *aTarget) = 0;

  /* readonly attribute nsIDOMEventTarget currentTarget; */
  NS_IMETHOD GetCurrentTarget(nsIDOMEventTarget * *aCurrentTarget) = 0;

  /* readonly attribute unsigned short eventPhase; */
  NS_IMETHOD GetEventPhase(PRUint16 *aEventPhase) = 0;

  /* readonly attribute boolean bubbles; */
  NS_IMETHOD GetBubbles(PRBool *aBubbles) = 0;

  /* readonly attribute boolean cancelable; */
  NS_IMETHOD GetCancelable(PRBool *aCancelable) = 0;

  /* readonly attribute DOMTimeStamp timeStamp; */
  NS_IMETHOD GetTimeStamp(DOMTimeStamp * *aTimeStamp) = 0;

  /* void stopPropagation (); */
  NS_IMETHOD StopPropagation(void) = 0;

  /* void preventDefault (); */
  NS_IMETHOD PreventDefault(void) = 0;

  /* void initEvent (in DOMString event_type_arg, in boolean can_bubble_arg, in boolean cancelable_arg); */
  NS_IMETHOD InitEvent(const nsAString & event_type_arg, PRBool can_bubble_arg, PRBool cancelable_arg) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMEVENT \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD GetTarget(nsIDOMEventTarget * *aTarget); \
  NS_IMETHOD GetCurrentTarget(nsIDOMEventTarget * *aCurrentTarget); \
  NS_IMETHOD GetEventPhase(PRUint16 *aEventPhase); \
  NS_IMETHOD GetBubbles(PRBool *aBubbles); \
  NS_IMETHOD GetCancelable(PRBool *aCancelable); \
  NS_IMETHOD GetTimeStamp(DOMTimeStamp * *aTimeStamp); \
  NS_IMETHOD StopPropagation(void); \
  NS_IMETHOD PreventDefault(void); \
  NS_IMETHOD InitEvent(const nsAString & event_type_arg, PRBool can_bubble_arg, PRBool cancelable_arg); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMEVENT(_to) \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD GetTarget(nsIDOMEventTarget * *aTarget) { return _to GetTarget(aTarget); } \
  NS_IMETHOD GetCurrentTarget(nsIDOMEventTarget * *aCurrentTarget) { return _to GetCurrentTarget(aCurrentTarget); } \
  NS_IMETHOD GetEventPhase(PRUint16 *aEventPhase) { return _to GetEventPhase(aEventPhase); } \
  NS_IMETHOD GetBubbles(PRBool *aBubbles) { return _to GetBubbles(aBubbles); } \
  NS_IMETHOD GetCancelable(PRBool *aCancelable) { return _to GetCancelable(aCancelable); } \
  NS_IMETHOD GetTimeStamp(DOMTimeStamp * *aTimeStamp) { return _to GetTimeStamp(aTimeStamp); } \
  NS_IMETHOD StopPropagation(void) { return _to StopPropagation(); } \
  NS_IMETHOD PreventDefault(void) { return _to PreventDefault(); } \
  NS_IMETHOD InitEvent(const nsAString & event_type_arg, PRBool can_bubble_arg, PRBool cancelable_arg) { return _to InitEvent(event_type_arg, can_bubble_arg, cancelable_arg); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMEVENT(_to) \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD GetTarget(nsIDOMEventTarget * *aTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTarget(aTarget); } \
  NS_IMETHOD GetCurrentTarget(nsIDOMEventTarget * *aCurrentTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCurrentTarget(aCurrentTarget); } \
  NS_IMETHOD GetEventPhase(PRUint16 *aEventPhase) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetEventPhase(aEventPhase); } \
  NS_IMETHOD GetBubbles(PRBool *aBubbles) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBubbles(aBubbles); } \
  NS_IMETHOD GetCancelable(PRBool *aCancelable) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCancelable(aCancelable); } \
  NS_IMETHOD GetTimeStamp(DOMTimeStamp * *aTimeStamp) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTimeStamp(aTimeStamp); } \
  NS_IMETHOD StopPropagation(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->StopPropagation(); } \
  NS_IMETHOD PreventDefault(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->PreventDefault(); } \
  NS_IMETHOD InitEvent(const nsAString & event_type_arg, PRBool can_bubble_arg, PRBool cancelable_arg) { return !_to ? NS_ERROR_NULL_POINTER : _to->InitEvent(event_type_arg, can_bubble_arg, cancelable_arg); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMEvent : public nsIDOMEvent
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMEVENT

  nsDOMEvent();

private:
  ~nsDOMEvent();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMEvent, nsIDOMEvent)

nsDOMEvent::nsDOMEvent()
{
  /* member initializers and constructor code */
}

nsDOMEvent::~nsDOMEvent()
{
  /* destructor code */
}

/* readonly attribute DOMString type; */
NS_IMETHODIMP nsDOMEvent::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMEventTarget target; */
NS_IMETHODIMP nsDOMEvent::GetTarget(nsIDOMEventTarget * *aTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMEventTarget currentTarget; */
NS_IMETHODIMP nsDOMEvent::GetCurrentTarget(nsIDOMEventTarget * *aCurrentTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned short eventPhase; */
NS_IMETHODIMP nsDOMEvent::GetEventPhase(PRUint16 *aEventPhase)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean bubbles; */
NS_IMETHODIMP nsDOMEvent::GetBubbles(PRBool *aBubbles)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean cancelable; */
NS_IMETHODIMP nsDOMEvent::GetCancelable(PRBool *aCancelable)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute DOMTimeStamp timeStamp; */
NS_IMETHODIMP nsDOMEvent::GetTimeStamp(DOMTimeStamp * *aTimeStamp)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void stopPropagation (); */
NS_IMETHODIMP nsDOMEvent::StopPropagation()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void preventDefault (); */
NS_IMETHODIMP nsDOMEvent::PreventDefault()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void initEvent (in DOMString event_type_arg, in boolean can_bubble_arg, in boolean cancelable_arg); */
NS_IMETHODIMP nsDOMEvent::InitEvent(const nsAString & event_type_arg, PRBool can_bubble_arg, PRBool cancelable_arg)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMEvent; /* forward declaration */


/* starting interface:    nsIDOMEventListener */
#define NS_IDOMEVENTLISTENER_IID_STR "df31c120-ded6-11d1-bd85-00805f8ae3f4"

#define NS_IDOMEVENTLISTENER_IID \
  {0xdf31c120, 0xded6, 0x11d1, \
    { 0xbd, 0x85, 0x00, 0x80, 0x5f, 0x8a, 0xe3, 0xf4 }}

class NS_NO_VTABLE nsIDOMEventListener : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMEVENTLISTENER_IID)

  /* void handleEvent (in nsIDOMEvent event); */
  NS_IMETHOD HandleEvent(nsIDOMEvent *event) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMEVENTLISTENER \
  NS_IMETHOD HandleEvent(nsIDOMEvent *event); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMEVENTLISTENER(_to) \
  NS_IMETHOD HandleEvent(nsIDOMEvent *event) { return _to HandleEvent(event); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMEVENTLISTENER(_to) \
  NS_IMETHOD HandleEvent(nsIDOMEvent *event) { return !_to ? NS_ERROR_NULL_POINTER : _to->HandleEvent(event); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMEventListener : public nsIDOMEventListener
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMEVENTLISTENER

  nsDOMEventListener();

private:
  ~nsDOMEventListener();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMEventListener, nsIDOMEventListener)

nsDOMEventListener::nsDOMEventListener()
{
  /* member initializers and constructor code */
}

nsDOMEventListener::~nsDOMEventListener()
{
  /* destructor code */
}

/* void handleEvent (in nsIDOMEvent event); */
NS_IMETHODIMP nsDOMEventListener::HandleEvent(nsIDOMEvent *event)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMEvent; /* forward declaration */

class nsIDOMEventListener; /* forward declaration */

class DOMException; /* forward declaration */


/* starting interface:    nsIDOMEventTarget */
#define NS_IDOMEVENTTARGET_IID_STR "1c773b30-d1cf-11d2-bd95-00805f8ae3f4"

#define NS_IDOMEVENTTARGET_IID \
  {0x1c773b30, 0xd1cf, 0x11d2, \
    { 0xbd, 0x95, 0x00, 0x80, 0x5f, 0x8a, 0xe3, 0xf4 }}

class NS_NO_VTABLE nsIDOMEventTarget : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMEVENTTARGET_IID)

  /* void addEventListener (in DOMString type, in nsIDOMEventListener listener, in boolean use_capture); */
  NS_IMETHOD AddEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture) = 0;

  /* void removeEventListener (in DOMString type, in nsIDOMEventListener listener, in boolean use_capture); */
  NS_IMETHOD RemoveEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture) = 0;

  /* boolean dispatchEvent (in nsIDOMEvent evt)  raises (DOMException); */
  NS_IMETHOD DispatchEvent(nsIDOMEvent *evt, PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMEVENTTARGET \
  NS_IMETHOD AddEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture); \
  NS_IMETHOD RemoveEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture); \
  NS_IMETHOD DispatchEvent(nsIDOMEvent *evt, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMEVENTTARGET(_to) \
  NS_IMETHOD AddEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture) { return _to AddEventListener(type, listener, use_capture); } \
  NS_IMETHOD RemoveEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture) { return _to RemoveEventListener(type, listener, use_capture); } \
  NS_IMETHOD DispatchEvent(nsIDOMEvent *evt, PRBool *_retval) { return _to DispatchEvent(evt, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMEVENTTARGET(_to) \
  NS_IMETHOD AddEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture) { return !_to ? NS_ERROR_NULL_POINTER : _to->AddEventListener(type, listener, use_capture); } \
  NS_IMETHOD RemoveEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveEventListener(type, listener, use_capture); } \
  NS_IMETHOD DispatchEvent(nsIDOMEvent *evt, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->DispatchEvent(evt, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMEventTarget : public nsIDOMEventTarget
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMEVENTTARGET

  nsDOMEventTarget();

private:
  ~nsDOMEventTarget();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMEventTarget, nsIDOMEventTarget)

nsDOMEventTarget::nsDOMEventTarget()
{
  /* member initializers and constructor code */
}

nsDOMEventTarget::~nsDOMEventTarget()
{
  /* destructor code */
}

/* void addEventListener (in DOMString type, in nsIDOMEventListener listener, in boolean use_capture); */
NS_IMETHODIMP nsDOMEventTarget::AddEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeEventListener (in DOMString type, in nsIDOMEventListener listener, in boolean use_capture); */
NS_IMETHODIMP nsDOMEventTarget::RemoveEventListener(const nsAString & type, nsIDOMEventListener *listener, PRBool use_capture)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean dispatchEvent (in nsIDOMEvent evt)  raises (DOMException); */
NS_IMETHODIMP nsDOMEventTarget::DispatchEvent(nsIDOMEvent *evt, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDOMHTMLElement */
#define NS_IDOMHTMLELEMENT_IID_STR "a6cf9085-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLELEMENT_IID \
  {0xa6cf9085, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLElement : public nsIDOMElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLELEMENT_IID)

  /* attribute DOMString id; */
  NS_IMETHOD GetId(nsAString & aId) = 0;
  NS_IMETHOD SetId(const nsAString & aId) = 0;

  /* attribute DOMString title; */
  NS_IMETHOD GetTitle(nsAString & aTitle) = 0;
  NS_IMETHOD SetTitle(const nsAString & aTitle) = 0;

  /* attribute DOMString lang; */
  NS_IMETHOD GetLang(nsAString & aLang) = 0;
  NS_IMETHOD SetLang(const nsAString & aLang) = 0;

  /* attribute DOMString dir; */
  NS_IMETHOD GetDir(nsAString & aDir) = 0;
  NS_IMETHOD SetDir(const nsAString & aDir) = 0;

  /* attribute DOMString className; */
  NS_IMETHOD GetClassName(nsAString & aClassName) = 0;
  NS_IMETHOD SetClassName(const nsAString & aClassName) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLELEMENT \
  NS_IMETHOD GetId(nsAString & aId); \
  NS_IMETHOD SetId(const nsAString & aId); \
  NS_IMETHOD GetTitle(nsAString & aTitle); \
  NS_IMETHOD SetTitle(const nsAString & aTitle); \
  NS_IMETHOD GetLang(nsAString & aLang); \
  NS_IMETHOD SetLang(const nsAString & aLang); \
  NS_IMETHOD GetDir(nsAString & aDir); \
  NS_IMETHOD SetDir(const nsAString & aDir); \
  NS_IMETHOD GetClassName(nsAString & aClassName); \
  NS_IMETHOD SetClassName(const nsAString & aClassName); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLELEMENT(_to) \
  NS_IMETHOD GetId(nsAString & aId) { return _to GetId(aId); } \
  NS_IMETHOD SetId(const nsAString & aId) { return _to SetId(aId); } \
  NS_IMETHOD GetTitle(nsAString & aTitle) { return _to GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const nsAString & aTitle) { return _to SetTitle(aTitle); } \
  NS_IMETHOD GetLang(nsAString & aLang) { return _to GetLang(aLang); } \
  NS_IMETHOD SetLang(const nsAString & aLang) { return _to SetLang(aLang); } \
  NS_IMETHOD GetDir(nsAString & aDir) { return _to GetDir(aDir); } \
  NS_IMETHOD SetDir(const nsAString & aDir) { return _to SetDir(aDir); } \
  NS_IMETHOD GetClassName(nsAString & aClassName) { return _to GetClassName(aClassName); } \
  NS_IMETHOD SetClassName(const nsAString & aClassName) { return _to SetClassName(aClassName); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLELEMENT(_to) \
  NS_IMETHOD GetId(nsAString & aId) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetId(aId); } \
  NS_IMETHOD SetId(const nsAString & aId) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetId(aId); } \
  NS_IMETHOD GetTitle(nsAString & aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const nsAString & aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTitle(aTitle); } \
  NS_IMETHOD GetLang(nsAString & aLang) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLang(aLang); } \
  NS_IMETHOD SetLang(const nsAString & aLang) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLang(aLang); } \
  NS_IMETHOD GetDir(nsAString & aDir) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDir(aDir); } \
  NS_IMETHOD SetDir(const nsAString & aDir) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDir(aDir); } \
  NS_IMETHOD GetClassName(nsAString & aClassName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetClassName(aClassName); } \
  NS_IMETHOD SetClassName(const nsAString & aClassName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetClassName(aClassName); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLElement : public nsIDOMHTMLElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLELEMENT

  nsDOMHTMLElement();

private:
  ~nsDOMHTMLElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLElement, nsIDOMHTMLElement)

nsDOMHTMLElement::nsDOMHTMLElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLElement::~nsDOMHTMLElement()
{
  /* destructor code */
}

/* attribute DOMString id; */
NS_IMETHODIMP nsDOMHTMLElement::GetId(nsAString & aId)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLElement::SetId(const nsAString & aId)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString title; */
NS_IMETHODIMP nsDOMHTMLElement::GetTitle(nsAString & aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLElement::SetTitle(const nsAString & aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString lang; */
NS_IMETHODIMP nsDOMHTMLElement::GetLang(nsAString & aLang)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLElement::SetLang(const nsAString & aLang)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString dir; */
NS_IMETHODIMP nsDOMHTMLElement::GetDir(nsAString & aDir)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLElement::SetDir(const nsAString & aDir)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString className; */
NS_IMETHODIMP nsDOMHTMLElement::GetClassName(nsAString & aClassName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLElement::SetClassName(const nsAString & aClassName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDOMHTMLAnchorElement */
#define NS_IDOMHTMLANCHORELEMENT_IID_STR "a6cf90aa-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLANCHORELEMENT_IID \
  {0xa6cf90aa, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLAnchorElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLANCHORELEMENT_IID)

  /* attribute DOMString accessKey; */
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) = 0;
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) = 0;

  /* attribute DOMString charset; */
  NS_IMETHOD GetCharset(nsAString & aCharset) = 0;
  NS_IMETHOD SetCharset(const nsAString & aCharset) = 0;

  /* attribute DOMString coords; */
  NS_IMETHOD GetCoords(nsAString & aCoords) = 0;
  NS_IMETHOD SetCoords(const nsAString & aCoords) = 0;

  /* attribute DOMString href; */
  NS_IMETHOD GetHref(nsAString & aHref) = 0;
  NS_IMETHOD SetHref(const nsAString & aHref) = 0;

  /* attribute DOMString hreflang; */
  NS_IMETHOD GetHreflang(nsAString & aHreflang) = 0;
  NS_IMETHOD SetHreflang(const nsAString & aHreflang) = 0;

  /* attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;
  NS_IMETHOD SetName(const nsAString & aName) = 0;

  /* attribute DOMString rel; */
  NS_IMETHOD GetRel(nsAString & aRel) = 0;
  NS_IMETHOD SetRel(const nsAString & aRel) = 0;

  /* attribute DOMString rev; */
  NS_IMETHOD GetRev(nsAString & aRev) = 0;
  NS_IMETHOD SetRev(const nsAString & aRev) = 0;

  /* attribute DOMString shape; */
  NS_IMETHOD GetShape(nsAString & aShape) = 0;
  NS_IMETHOD SetShape(const nsAString & aShape) = 0;

  /* attribute long tabIndex; */
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) = 0;
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) = 0;

  /* attribute DOMString target; */
  NS_IMETHOD GetTarget(nsAString & aTarget) = 0;
  NS_IMETHOD SetTarget(const nsAString & aTarget) = 0;

  /* attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;
  NS_IMETHOD SetType(const nsAString & aType) = 0;

  /* void blur (); */
  NS_IMETHOD Blur(void) = 0;

  /* void focus (); */
  NS_IMETHOD Focus(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLANCHORELEMENT \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey); \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey); \
  NS_IMETHOD GetCharset(nsAString & aCharset); \
  NS_IMETHOD SetCharset(const nsAString & aCharset); \
  NS_IMETHOD GetCoords(nsAString & aCoords); \
  NS_IMETHOD SetCoords(const nsAString & aCoords); \
  NS_IMETHOD GetHref(nsAString & aHref); \
  NS_IMETHOD SetHref(const nsAString & aHref); \
  NS_IMETHOD GetHreflang(nsAString & aHreflang); \
  NS_IMETHOD SetHreflang(const nsAString & aHreflang); \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD SetName(const nsAString & aName); \
  NS_IMETHOD GetRel(nsAString & aRel); \
  NS_IMETHOD SetRel(const nsAString & aRel); \
  NS_IMETHOD GetRev(nsAString & aRev); \
  NS_IMETHOD SetRev(const nsAString & aRev); \
  NS_IMETHOD GetShape(nsAString & aShape); \
  NS_IMETHOD SetShape(const nsAString & aShape); \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex); \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex); \
  NS_IMETHOD GetTarget(nsAString & aTarget); \
  NS_IMETHOD SetTarget(const nsAString & aTarget); \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD SetType(const nsAString & aType); \
  NS_IMETHOD Blur(void); \
  NS_IMETHOD Focus(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLANCHORELEMENT(_to) \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return _to GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return _to SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetCharset(nsAString & aCharset) { return _to GetCharset(aCharset); } \
  NS_IMETHOD SetCharset(const nsAString & aCharset) { return _to SetCharset(aCharset); } \
  NS_IMETHOD GetCoords(nsAString & aCoords) { return _to GetCoords(aCoords); } \
  NS_IMETHOD SetCoords(const nsAString & aCoords) { return _to SetCoords(aCoords); } \
  NS_IMETHOD GetHref(nsAString & aHref) { return _to GetHref(aHref); } \
  NS_IMETHOD SetHref(const nsAString & aHref) { return _to SetHref(aHref); } \
  NS_IMETHOD GetHreflang(nsAString & aHreflang) { return _to GetHreflang(aHreflang); } \
  NS_IMETHOD SetHreflang(const nsAString & aHreflang) { return _to SetHreflang(aHreflang); } \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return _to SetName(aName); } \
  NS_IMETHOD GetRel(nsAString & aRel) { return _to GetRel(aRel); } \
  NS_IMETHOD SetRel(const nsAString & aRel) { return _to SetRel(aRel); } \
  NS_IMETHOD GetRev(nsAString & aRev) { return _to GetRev(aRev); } \
  NS_IMETHOD SetRev(const nsAString & aRev) { return _to SetRev(aRev); } \
  NS_IMETHOD GetShape(nsAString & aShape) { return _to GetShape(aShape); } \
  NS_IMETHOD SetShape(const nsAString & aShape) { return _to SetShape(aShape); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return _to GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return _to SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetTarget(nsAString & aTarget) { return _to GetTarget(aTarget); } \
  NS_IMETHOD SetTarget(const nsAString & aTarget) { return _to SetTarget(aTarget); } \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return _to SetType(aType); } \
  NS_IMETHOD Blur(void) { return _to Blur(); } \
  NS_IMETHOD Focus(void) { return _to Focus(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLANCHORELEMENT(_to) \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetCharset(nsAString & aCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCharset(aCharset); } \
  NS_IMETHOD SetCharset(const nsAString & aCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetCharset(aCharset); } \
  NS_IMETHOD GetCoords(nsAString & aCoords) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCoords(aCoords); } \
  NS_IMETHOD SetCoords(const nsAString & aCoords) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetCoords(aCoords); } \
  NS_IMETHOD GetHref(nsAString & aHref) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHref(aHref); } \
  NS_IMETHOD SetHref(const nsAString & aHref) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHref(aHref); } \
  NS_IMETHOD GetHreflang(nsAString & aHreflang) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHreflang(aHreflang); } \
  NS_IMETHOD SetHreflang(const nsAString & aHreflang) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHreflang(aHreflang); } \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD GetRel(nsAString & aRel) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRel(aRel); } \
  NS_IMETHOD SetRel(const nsAString & aRel) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetRel(aRel); } \
  NS_IMETHOD GetRev(nsAString & aRev) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRev(aRev); } \
  NS_IMETHOD SetRev(const nsAString & aRev) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetRev(aRev); } \
  NS_IMETHOD GetShape(nsAString & aShape) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetShape(aShape); } \
  NS_IMETHOD SetShape(const nsAString & aShape) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetShape(aShape); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetTarget(nsAString & aTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTarget(aTarget); } \
  NS_IMETHOD SetTarget(const nsAString & aTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTarget(aTarget); } \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetType(aType); } \
  NS_IMETHOD Blur(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Blur(); } \
  NS_IMETHOD Focus(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Focus(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLAnchorElement : public nsIDOMHTMLAnchorElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLANCHORELEMENT

  nsDOMHTMLAnchorElement();

private:
  ~nsDOMHTMLAnchorElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLAnchorElement, nsIDOMHTMLAnchorElement)

nsDOMHTMLAnchorElement::nsDOMHTMLAnchorElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLAnchorElement::~nsDOMHTMLAnchorElement()
{
  /* destructor code */
}

/* attribute DOMString accessKey; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetAccessKey(nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetAccessKey(const nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString charset; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetCharset(nsAString & aCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetCharset(const nsAString & aCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString coords; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetCoords(nsAString & aCoords)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetCoords(const nsAString & aCoords)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString href; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetHref(nsAString & aHref)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetHref(const nsAString & aHref)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString hreflang; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetHreflang(nsAString & aHreflang)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetHreflang(const nsAString & aHreflang)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString name; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetName(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString rel; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetRel(nsAString & aRel)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetRel(const nsAString & aRel)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString rev; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetRev(nsAString & aRev)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetRev(const nsAString & aRev)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString shape; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetShape(nsAString & aShape)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetShape(const nsAString & aShape)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long tabIndex; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetTabIndex(PRInt32 *aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetTabIndex(PRInt32 aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString target; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetTarget(nsAString & aTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetTarget(const nsAString & aTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString type; */
NS_IMETHODIMP nsDOMHTMLAnchorElement::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLAnchorElement::SetType(const nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void blur (); */
NS_IMETHODIMP nsDOMHTMLAnchorElement::Blur()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void focus (); */
NS_IMETHODIMP nsDOMHTMLAnchorElement::Focus()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMHTMLFormElement; /* forward declaration */


/* starting interface:    nsIDOMHTMLButtonElement */
#define NS_IDOMHTMLBUTTONELEMENT_IID_STR "a6cf9095-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLBUTTONELEMENT_IID \
  {0xa6cf9095, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLButtonElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLBUTTONELEMENT_IID)

  /* readonly attribute nsIDOMHTMLFormElement form; */
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) = 0;

  /* attribute DOMString accessKey; */
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) = 0;
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) = 0;

  /* attribute boolean disabled; */
  NS_IMETHOD GetDisabled(PRBool *aDisabled) = 0;
  NS_IMETHOD SetDisabled(PRBool aDisabled) = 0;

  /* attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;
  NS_IMETHOD SetName(const nsAString & aName) = 0;

  /* attribute long tabIndex; */
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) = 0;
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) = 0;

  /* [noscript] readonly attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;

  /* attribute DOMString value; */
  NS_IMETHOD GetValue(nsAString & aValue) = 0;
  NS_IMETHOD SetValue(const nsAString & aValue) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLBUTTONELEMENT \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm); \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey); \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey); \
  NS_IMETHOD GetDisabled(PRBool *aDisabled); \
  NS_IMETHOD SetDisabled(PRBool aDisabled); \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD SetName(const nsAString & aName); \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex); \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex); \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD GetValue(nsAString & aValue); \
  NS_IMETHOD SetValue(const nsAString & aValue); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLBUTTONELEMENT(_to) \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return _to GetForm(aForm); } \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return _to GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return _to SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return _to GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return _to SetDisabled(aDisabled); } \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return _to SetName(aName); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return _to GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return _to SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return _to GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return _to SetValue(aValue); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLBUTTONELEMENT(_to) \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetForm(aForm); } \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDisabled(aDisabled); } \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValue(aValue); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLButtonElement : public nsIDOMHTMLButtonElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLBUTTONELEMENT

  nsDOMHTMLButtonElement();

private:
  ~nsDOMHTMLButtonElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLButtonElement, nsIDOMHTMLButtonElement)

nsDOMHTMLButtonElement::nsDOMHTMLButtonElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLButtonElement::~nsDOMHTMLButtonElement()
{
  /* destructor code */
}

/* readonly attribute nsIDOMHTMLFormElement form; */
NS_IMETHODIMP nsDOMHTMLButtonElement::GetForm(nsIDOMHTMLFormElement * *aForm)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString accessKey; */
NS_IMETHODIMP nsDOMHTMLButtonElement::GetAccessKey(nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLButtonElement::SetAccessKey(const nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean disabled; */
NS_IMETHODIMP nsDOMHTMLButtonElement::GetDisabled(PRBool *aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLButtonElement::SetDisabled(PRBool aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString name; */
NS_IMETHODIMP nsDOMHTMLButtonElement::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLButtonElement::SetName(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long tabIndex; */
NS_IMETHODIMP nsDOMHTMLButtonElement::GetTabIndex(PRInt32 *aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLButtonElement::SetTabIndex(PRInt32 aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] readonly attribute DOMString type; */
NS_IMETHODIMP nsDOMHTMLButtonElement::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString value; */
NS_IMETHODIMP nsDOMHTMLButtonElement::GetValue(nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLButtonElement::SetValue(const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMHTMLFormElement; /* forward declaration */


/* starting interface:    nsIDOMHTMLInputElement */
#define NS_IDOMHTMLINPUTELEMENT_IID_STR "a6cf9093-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLINPUTELEMENT_IID \
  {0xa6cf9093, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLInputElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLINPUTELEMENT_IID)

  /* attribute DOMString defaultValue; */
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue) = 0;
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue) = 0;

  /* attribute boolean defaultChecked; */
  NS_IMETHOD GetDefaultChecked(PRBool *aDefaultChecked) = 0;
  NS_IMETHOD SetDefaultChecked(PRBool aDefaultChecked) = 0;

  /* readonly attribute nsIDOMHTMLFormElement form; */
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) = 0;

  /* attribute DOMString accept; */
  NS_IMETHOD GetAccept(nsAString & aAccept) = 0;
  NS_IMETHOD SetAccept(const nsAString & aAccept) = 0;

  /* attribute DOMString accessKey; */
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) = 0;
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) = 0;

  /* attribute DOMString align; */
  NS_IMETHOD GetAlign(nsAString & aAlign) = 0;
  NS_IMETHOD SetAlign(const nsAString & aAlign) = 0;

  /* attribute DOMString alt; */
  NS_IMETHOD GetAlt(nsAString & aAlt) = 0;
  NS_IMETHOD SetAlt(const nsAString & aAlt) = 0;

  /* attribute boolean checked; */
  NS_IMETHOD GetChecked(PRBool *aChecked) = 0;
  NS_IMETHOD SetChecked(PRBool aChecked) = 0;

  /* attribute boolean disabled; */
  NS_IMETHOD GetDisabled(PRBool *aDisabled) = 0;
  NS_IMETHOD SetDisabled(PRBool aDisabled) = 0;

  /* attribute long maxLength; */
  NS_IMETHOD GetMaxLength(PRInt32 *aMaxLength) = 0;
  NS_IMETHOD SetMaxLength(PRInt32 aMaxLength) = 0;

  /* attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;
  NS_IMETHOD SetName(const nsAString & aName) = 0;

  /* attribute boolean readOnly; */
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly) = 0;
  NS_IMETHOD SetReadOnly(PRBool aReadOnly) = 0;

  /* attribute unsigned long size; */
  NS_IMETHOD GetSize(PRUint32 *aSize) = 0;
  NS_IMETHOD SetSize(PRUint32 aSize) = 0;

  /* attribute DOMString src; */
  NS_IMETHOD GetSrc(nsAString & aSrc) = 0;
  NS_IMETHOD SetSrc(const nsAString & aSrc) = 0;

  /* attribute long tabIndex; */
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) = 0;
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) = 0;

  /* attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;
  NS_IMETHOD SetType(const nsAString & aType) = 0;

  /* attribute DOMString useMap; */
  NS_IMETHOD GetUseMap(nsAString & aUseMap) = 0;
  NS_IMETHOD SetUseMap(const nsAString & aUseMap) = 0;

  /* attribute DOMString value; */
  NS_IMETHOD GetValue(nsAString & aValue) = 0;
  NS_IMETHOD SetValue(const nsAString & aValue) = 0;

  /* void blur (); */
  NS_IMETHOD Blur(void) = 0;

  /* void focus (); */
  NS_IMETHOD Focus(void) = 0;

  /* void select (); */
  NS_IMETHOD Select(void) = 0;

  /* void click (); */
  NS_IMETHOD Click(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLINPUTELEMENT \
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue); \
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue); \
  NS_IMETHOD GetDefaultChecked(PRBool *aDefaultChecked); \
  NS_IMETHOD SetDefaultChecked(PRBool aDefaultChecked); \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm); \
  NS_IMETHOD GetAccept(nsAString & aAccept); \
  NS_IMETHOD SetAccept(const nsAString & aAccept); \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey); \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey); \
  NS_IMETHOD GetAlign(nsAString & aAlign); \
  NS_IMETHOD SetAlign(const nsAString & aAlign); \
  NS_IMETHOD GetAlt(nsAString & aAlt); \
  NS_IMETHOD SetAlt(const nsAString & aAlt); \
  NS_IMETHOD GetChecked(PRBool *aChecked); \
  NS_IMETHOD SetChecked(PRBool aChecked); \
  NS_IMETHOD GetDisabled(PRBool *aDisabled); \
  NS_IMETHOD SetDisabled(PRBool aDisabled); \
  NS_IMETHOD GetMaxLength(PRInt32 *aMaxLength); \
  NS_IMETHOD SetMaxLength(PRInt32 aMaxLength); \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD SetName(const nsAString & aName); \
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly); \
  NS_IMETHOD SetReadOnly(PRBool aReadOnly); \
  NS_IMETHOD GetSize(PRUint32 *aSize); \
  NS_IMETHOD SetSize(PRUint32 aSize); \
  NS_IMETHOD GetSrc(nsAString & aSrc); \
  NS_IMETHOD SetSrc(const nsAString & aSrc); \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex); \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex); \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD SetType(const nsAString & aType); \
  NS_IMETHOD GetUseMap(nsAString & aUseMap); \
  NS_IMETHOD SetUseMap(const nsAString & aUseMap); \
  NS_IMETHOD GetValue(nsAString & aValue); \
  NS_IMETHOD SetValue(const nsAString & aValue); \
  NS_IMETHOD Blur(void); \
  NS_IMETHOD Focus(void); \
  NS_IMETHOD Select(void); \
  NS_IMETHOD Click(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLINPUTELEMENT(_to) \
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue) { return _to GetDefaultValue(aDefaultValue); } \
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue) { return _to SetDefaultValue(aDefaultValue); } \
  NS_IMETHOD GetDefaultChecked(PRBool *aDefaultChecked) { return _to GetDefaultChecked(aDefaultChecked); } \
  NS_IMETHOD SetDefaultChecked(PRBool aDefaultChecked) { return _to SetDefaultChecked(aDefaultChecked); } \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return _to GetForm(aForm); } \
  NS_IMETHOD GetAccept(nsAString & aAccept) { return _to GetAccept(aAccept); } \
  NS_IMETHOD SetAccept(const nsAString & aAccept) { return _to SetAccept(aAccept); } \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return _to GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return _to SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetAlign(nsAString & aAlign) { return _to GetAlign(aAlign); } \
  NS_IMETHOD SetAlign(const nsAString & aAlign) { return _to SetAlign(aAlign); } \
  NS_IMETHOD GetAlt(nsAString & aAlt) { return _to GetAlt(aAlt); } \
  NS_IMETHOD SetAlt(const nsAString & aAlt) { return _to SetAlt(aAlt); } \
  NS_IMETHOD GetChecked(PRBool *aChecked) { return _to GetChecked(aChecked); } \
  NS_IMETHOD SetChecked(PRBool aChecked) { return _to SetChecked(aChecked); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return _to GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return _to SetDisabled(aDisabled); } \
  NS_IMETHOD GetMaxLength(PRInt32 *aMaxLength) { return _to GetMaxLength(aMaxLength); } \
  NS_IMETHOD SetMaxLength(PRInt32 aMaxLength) { return _to SetMaxLength(aMaxLength); } \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return _to SetName(aName); } \
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly) { return _to GetReadOnly(aReadOnly); } \
  NS_IMETHOD SetReadOnly(PRBool aReadOnly) { return _to SetReadOnly(aReadOnly); } \
  NS_IMETHOD GetSize(PRUint32 *aSize) { return _to GetSize(aSize); } \
  NS_IMETHOD SetSize(PRUint32 aSize) { return _to SetSize(aSize); } \
  NS_IMETHOD GetSrc(nsAString & aSrc) { return _to GetSrc(aSrc); } \
  NS_IMETHOD SetSrc(const nsAString & aSrc) { return _to SetSrc(aSrc); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return _to GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return _to SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return _to SetType(aType); } \
  NS_IMETHOD GetUseMap(nsAString & aUseMap) { return _to GetUseMap(aUseMap); } \
  NS_IMETHOD SetUseMap(const nsAString & aUseMap) { return _to SetUseMap(aUseMap); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return _to GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return _to SetValue(aValue); } \
  NS_IMETHOD Blur(void) { return _to Blur(); } \
  NS_IMETHOD Focus(void) { return _to Focus(); } \
  NS_IMETHOD Select(void) { return _to Select(); } \
  NS_IMETHOD Click(void) { return _to Click(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLINPUTELEMENT(_to) \
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultValue(aDefaultValue); } \
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultValue(aDefaultValue); } \
  NS_IMETHOD GetDefaultChecked(PRBool *aDefaultChecked) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultChecked(aDefaultChecked); } \
  NS_IMETHOD SetDefaultChecked(PRBool aDefaultChecked) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultChecked(aDefaultChecked); } \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetForm(aForm); } \
  NS_IMETHOD GetAccept(nsAString & aAccept) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAccept(aAccept); } \
  NS_IMETHOD SetAccept(const nsAString & aAccept) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAccept(aAccept); } \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetAlign(nsAString & aAlign) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAlign(aAlign); } \
  NS_IMETHOD SetAlign(const nsAString & aAlign) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAlign(aAlign); } \
  NS_IMETHOD GetAlt(nsAString & aAlt) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAlt(aAlt); } \
  NS_IMETHOD SetAlt(const nsAString & aAlt) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAlt(aAlt); } \
  NS_IMETHOD GetChecked(PRBool *aChecked) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChecked(aChecked); } \
  NS_IMETHOD SetChecked(PRBool aChecked) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetChecked(aChecked); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDisabled(aDisabled); } \
  NS_IMETHOD GetMaxLength(PRInt32 *aMaxLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMaxLength(aMaxLength); } \
  NS_IMETHOD SetMaxLength(PRInt32 aMaxLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMaxLength(aMaxLength); } \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetReadOnly(aReadOnly); } \
  NS_IMETHOD SetReadOnly(PRBool aReadOnly) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetReadOnly(aReadOnly); } \
  NS_IMETHOD GetSize(PRUint32 *aSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSize(aSize); } \
  NS_IMETHOD SetSize(PRUint32 aSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSize(aSize); } \
  NS_IMETHOD GetSrc(nsAString & aSrc) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSrc(aSrc); } \
  NS_IMETHOD SetSrc(const nsAString & aSrc) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSrc(aSrc); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetType(aType); } \
  NS_IMETHOD GetUseMap(nsAString & aUseMap) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetUseMap(aUseMap); } \
  NS_IMETHOD SetUseMap(const nsAString & aUseMap) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetUseMap(aUseMap); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValue(aValue); } \
  NS_IMETHOD Blur(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Blur(); } \
  NS_IMETHOD Focus(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Focus(); } \
  NS_IMETHOD Select(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Select(); } \
  NS_IMETHOD Click(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Click(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLInputElement : public nsIDOMHTMLInputElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLINPUTELEMENT

  nsDOMHTMLInputElement();

private:
  ~nsDOMHTMLInputElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLInputElement, nsIDOMHTMLInputElement)

nsDOMHTMLInputElement::nsDOMHTMLInputElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLInputElement::~nsDOMHTMLInputElement()
{
  /* destructor code */
}

/* attribute DOMString defaultValue; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetDefaultValue(nsAString & aDefaultValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetDefaultValue(const nsAString & aDefaultValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean defaultChecked; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetDefaultChecked(PRBool *aDefaultChecked)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetDefaultChecked(PRBool aDefaultChecked)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMHTMLFormElement form; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetForm(nsIDOMHTMLFormElement * *aForm)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString accept; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetAccept(nsAString & aAccept)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetAccept(const nsAString & aAccept)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString accessKey; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetAccessKey(nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetAccessKey(const nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString align; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetAlign(nsAString & aAlign)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetAlign(const nsAString & aAlign)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString alt; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetAlt(nsAString & aAlt)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetAlt(const nsAString & aAlt)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean checked; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetChecked(PRBool *aChecked)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetChecked(PRBool aChecked)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean disabled; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetDisabled(PRBool *aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetDisabled(PRBool aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long maxLength; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetMaxLength(PRInt32 *aMaxLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetMaxLength(PRInt32 aMaxLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString name; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetName(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean readOnly; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetReadOnly(PRBool *aReadOnly)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetReadOnly(PRBool aReadOnly)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute unsigned long size; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetSize(PRUint32 *aSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetSize(PRUint32 aSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString src; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetSrc(nsAString & aSrc)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetSrc(const nsAString & aSrc)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long tabIndex; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetTabIndex(PRInt32 *aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetTabIndex(PRInt32 aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString type; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetType(const nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString useMap; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetUseMap(nsAString & aUseMap)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetUseMap(const nsAString & aUseMap)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString value; */
NS_IMETHODIMP nsDOMHTMLInputElement::GetValue(nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLInputElement::SetValue(const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void blur (); */
NS_IMETHODIMP nsDOMHTMLInputElement::Blur()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void focus (); */
NS_IMETHODIMP nsDOMHTMLInputElement::Focus()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void select (); */
NS_IMETHODIMP nsDOMHTMLInputElement::Select()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void click (); */
NS_IMETHODIMP nsDOMHTMLInputElement::Click()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDOMHTMLLinkElement */
#define NS_IDOMHTMLLINKELEMENT_IID_STR "a6cf9088-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLLINKELEMENT_IID \
  {0xa6cf9088, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLLinkElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLLINKELEMENT_IID)

  /* attribute boolean disabled; */
  NS_IMETHOD GetDisabled(PRBool *aDisabled) = 0;
  NS_IMETHOD SetDisabled(PRBool aDisabled) = 0;

  /* attribute DOMString charset; */
  NS_IMETHOD GetCharset(nsAString & aCharset) = 0;
  NS_IMETHOD SetCharset(const nsAString & aCharset) = 0;

  /* attribute DOMString href; */
  NS_IMETHOD GetHref(nsAString & aHref) = 0;
  NS_IMETHOD SetHref(const nsAString & aHref) = 0;

  /* attribute DOMString hreflang; */
  NS_IMETHOD GetHreflang(nsAString & aHreflang) = 0;
  NS_IMETHOD SetHreflang(const nsAString & aHreflang) = 0;

  /* attribute DOMString media; */
  NS_IMETHOD GetMedia(nsAString & aMedia) = 0;
  NS_IMETHOD SetMedia(const nsAString & aMedia) = 0;

  /* attribute DOMString rel; */
  NS_IMETHOD GetRel(nsAString & aRel) = 0;
  NS_IMETHOD SetRel(const nsAString & aRel) = 0;

  /* attribute DOMString rev; */
  NS_IMETHOD GetRev(nsAString & aRev) = 0;
  NS_IMETHOD SetRev(const nsAString & aRev) = 0;

  /* attribute DOMString target; */
  NS_IMETHOD GetTarget(nsAString & aTarget) = 0;
  NS_IMETHOD SetTarget(const nsAString & aTarget) = 0;

  /* attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;
  NS_IMETHOD SetType(const nsAString & aType) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLLINKELEMENT \
  NS_IMETHOD GetDisabled(PRBool *aDisabled); \
  NS_IMETHOD SetDisabled(PRBool aDisabled); \
  NS_IMETHOD GetCharset(nsAString & aCharset); \
  NS_IMETHOD SetCharset(const nsAString & aCharset); \
  NS_IMETHOD GetHref(nsAString & aHref); \
  NS_IMETHOD SetHref(const nsAString & aHref); \
  NS_IMETHOD GetHreflang(nsAString & aHreflang); \
  NS_IMETHOD SetHreflang(const nsAString & aHreflang); \
  NS_IMETHOD GetMedia(nsAString & aMedia); \
  NS_IMETHOD SetMedia(const nsAString & aMedia); \
  NS_IMETHOD GetRel(nsAString & aRel); \
  NS_IMETHOD SetRel(const nsAString & aRel); \
  NS_IMETHOD GetRev(nsAString & aRev); \
  NS_IMETHOD SetRev(const nsAString & aRev); \
  NS_IMETHOD GetTarget(nsAString & aTarget); \
  NS_IMETHOD SetTarget(const nsAString & aTarget); \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD SetType(const nsAString & aType); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLLINKELEMENT(_to) \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return _to GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return _to SetDisabled(aDisabled); } \
  NS_IMETHOD GetCharset(nsAString & aCharset) { return _to GetCharset(aCharset); } \
  NS_IMETHOD SetCharset(const nsAString & aCharset) { return _to SetCharset(aCharset); } \
  NS_IMETHOD GetHref(nsAString & aHref) { return _to GetHref(aHref); } \
  NS_IMETHOD SetHref(const nsAString & aHref) { return _to SetHref(aHref); } \
  NS_IMETHOD GetHreflang(nsAString & aHreflang) { return _to GetHreflang(aHreflang); } \
  NS_IMETHOD SetHreflang(const nsAString & aHreflang) { return _to SetHreflang(aHreflang); } \
  NS_IMETHOD GetMedia(nsAString & aMedia) { return _to GetMedia(aMedia); } \
  NS_IMETHOD SetMedia(const nsAString & aMedia) { return _to SetMedia(aMedia); } \
  NS_IMETHOD GetRel(nsAString & aRel) { return _to GetRel(aRel); } \
  NS_IMETHOD SetRel(const nsAString & aRel) { return _to SetRel(aRel); } \
  NS_IMETHOD GetRev(nsAString & aRev) { return _to GetRev(aRev); } \
  NS_IMETHOD SetRev(const nsAString & aRev) { return _to SetRev(aRev); } \
  NS_IMETHOD GetTarget(nsAString & aTarget) { return _to GetTarget(aTarget); } \
  NS_IMETHOD SetTarget(const nsAString & aTarget) { return _to SetTarget(aTarget); } \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return _to SetType(aType); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLLINKELEMENT(_to) \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDisabled(aDisabled); } \
  NS_IMETHOD GetCharset(nsAString & aCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCharset(aCharset); } \
  NS_IMETHOD SetCharset(const nsAString & aCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetCharset(aCharset); } \
  NS_IMETHOD GetHref(nsAString & aHref) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHref(aHref); } \
  NS_IMETHOD SetHref(const nsAString & aHref) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHref(aHref); } \
  NS_IMETHOD GetHreflang(nsAString & aHreflang) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHreflang(aHreflang); } \
  NS_IMETHOD SetHreflang(const nsAString & aHreflang) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHreflang(aHreflang); } \
  NS_IMETHOD GetMedia(nsAString & aMedia) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMedia(aMedia); } \
  NS_IMETHOD SetMedia(const nsAString & aMedia) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMedia(aMedia); } \
  NS_IMETHOD GetRel(nsAString & aRel) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRel(aRel); } \
  NS_IMETHOD SetRel(const nsAString & aRel) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetRel(aRel); } \
  NS_IMETHOD GetRev(nsAString & aRev) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRev(aRev); } \
  NS_IMETHOD SetRev(const nsAString & aRev) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetRev(aRev); } \
  NS_IMETHOD GetTarget(nsAString & aTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTarget(aTarget); } \
  NS_IMETHOD SetTarget(const nsAString & aTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTarget(aTarget); } \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetType(aType); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLLinkElement : public nsIDOMHTMLLinkElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLLINKELEMENT

  nsDOMHTMLLinkElement();

private:
  ~nsDOMHTMLLinkElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLLinkElement, nsIDOMHTMLLinkElement)

nsDOMHTMLLinkElement::nsDOMHTMLLinkElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLLinkElement::~nsDOMHTMLLinkElement()
{
  /* destructor code */
}

/* attribute boolean disabled; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetDisabled(PRBool *aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetDisabled(PRBool aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString charset; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetCharset(nsAString & aCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetCharset(const nsAString & aCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString href; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetHref(nsAString & aHref)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetHref(const nsAString & aHref)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString hreflang; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetHreflang(nsAString & aHreflang)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetHreflang(const nsAString & aHreflang)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString media; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetMedia(nsAString & aMedia)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetMedia(const nsAString & aMedia)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString rel; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetRel(nsAString & aRel)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetRel(const nsAString & aRel)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString rev; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetRev(nsAString & aRev)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetRev(const nsAString & aRev)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString target; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetTarget(nsAString & aTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetTarget(const nsAString & aTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString type; */
NS_IMETHODIMP nsDOMHTMLLinkElement::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLLinkElement::SetType(const nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMHTMLFormElement; /* forward declaration */


/* starting interface:    nsIDOMHTMLOptionElement */
#define NS_IDOMHTMLOPTIONELEMENT_IID_STR "a6cf9092-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLOPTIONELEMENT_IID \
  {0xa6cf9092, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLOptionElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLOPTIONELEMENT_IID)

  /* readonly attribute nsIDOMHTMLFormElement form; */
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) = 0;

  /* attribute boolean defaultSelected; */
  NS_IMETHOD GetDefaultSelected(PRBool *aDefaultSelected) = 0;
  NS_IMETHOD SetDefaultSelected(PRBool aDefaultSelected) = 0;

  /* readonly attribute DOMString text; */
  NS_IMETHOD GetText(nsAString & aText) = 0;

  /* readonly attribute long index; */
  NS_IMETHOD GetIndex(PRInt32 *aIndex) = 0;

  /* attribute boolean disabled; */
  NS_IMETHOD GetDisabled(PRBool *aDisabled) = 0;
  NS_IMETHOD SetDisabled(PRBool aDisabled) = 0;

  /* attribute DOMString label; */
  NS_IMETHOD GetLabel(nsAString & aLabel) = 0;
  NS_IMETHOD SetLabel(const nsAString & aLabel) = 0;

  /* attribute boolean selected; */
  NS_IMETHOD GetSelected(PRBool *aSelected) = 0;
  NS_IMETHOD SetSelected(PRBool aSelected) = 0;

  /* attribute DOMString value; */
  NS_IMETHOD GetValue(nsAString & aValue) = 0;
  NS_IMETHOD SetValue(const nsAString & aValue) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLOPTIONELEMENT \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm); \
  NS_IMETHOD GetDefaultSelected(PRBool *aDefaultSelected); \
  NS_IMETHOD SetDefaultSelected(PRBool aDefaultSelected); \
  NS_IMETHOD GetText(nsAString & aText); \
  NS_IMETHOD GetIndex(PRInt32 *aIndex); \
  NS_IMETHOD GetDisabled(PRBool *aDisabled); \
  NS_IMETHOD SetDisabled(PRBool aDisabled); \
  NS_IMETHOD GetLabel(nsAString & aLabel); \
  NS_IMETHOD SetLabel(const nsAString & aLabel); \
  NS_IMETHOD GetSelected(PRBool *aSelected); \
  NS_IMETHOD SetSelected(PRBool aSelected); \
  NS_IMETHOD GetValue(nsAString & aValue); \
  NS_IMETHOD SetValue(const nsAString & aValue); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLOPTIONELEMENT(_to) \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return _to GetForm(aForm); } \
  NS_IMETHOD GetDefaultSelected(PRBool *aDefaultSelected) { return _to GetDefaultSelected(aDefaultSelected); } \
  NS_IMETHOD SetDefaultSelected(PRBool aDefaultSelected) { return _to SetDefaultSelected(aDefaultSelected); } \
  NS_IMETHOD GetText(nsAString & aText) { return _to GetText(aText); } \
  NS_IMETHOD GetIndex(PRInt32 *aIndex) { return _to GetIndex(aIndex); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return _to GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return _to SetDisabled(aDisabled); } \
  NS_IMETHOD GetLabel(nsAString & aLabel) { return _to GetLabel(aLabel); } \
  NS_IMETHOD SetLabel(const nsAString & aLabel) { return _to SetLabel(aLabel); } \
  NS_IMETHOD GetSelected(PRBool *aSelected) { return _to GetSelected(aSelected); } \
  NS_IMETHOD SetSelected(PRBool aSelected) { return _to SetSelected(aSelected); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return _to GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return _to SetValue(aValue); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLOPTIONELEMENT(_to) \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetForm(aForm); } \
  NS_IMETHOD GetDefaultSelected(PRBool *aDefaultSelected) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultSelected(aDefaultSelected); } \
  NS_IMETHOD SetDefaultSelected(PRBool aDefaultSelected) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultSelected(aDefaultSelected); } \
  NS_IMETHOD GetText(nsAString & aText) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetText(aText); } \
  NS_IMETHOD GetIndex(PRInt32 *aIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIndex(aIndex); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDisabled(aDisabled); } \
  NS_IMETHOD GetLabel(nsAString & aLabel) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLabel(aLabel); } \
  NS_IMETHOD SetLabel(const nsAString & aLabel) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLabel(aLabel); } \
  NS_IMETHOD GetSelected(PRBool *aSelected) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSelected(aSelected); } \
  NS_IMETHOD SetSelected(PRBool aSelected) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSelected(aSelected); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValue(aValue); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLOptionElement : public nsIDOMHTMLOptionElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLOPTIONELEMENT

  nsDOMHTMLOptionElement();

private:
  ~nsDOMHTMLOptionElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLOptionElement, nsIDOMHTMLOptionElement)

nsDOMHTMLOptionElement::nsDOMHTMLOptionElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLOptionElement::~nsDOMHTMLOptionElement()
{
  /* destructor code */
}

/* readonly attribute nsIDOMHTMLFormElement form; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetForm(nsIDOMHTMLFormElement * *aForm)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean defaultSelected; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetDefaultSelected(PRBool *aDefaultSelected)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLOptionElement::SetDefaultSelected(PRBool aDefaultSelected)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute DOMString text; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetText(nsAString & aText)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long index; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetIndex(PRInt32 *aIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean disabled; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetDisabled(PRBool *aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLOptionElement::SetDisabled(PRBool aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString label; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetLabel(nsAString & aLabel)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLOptionElement::SetLabel(const nsAString & aLabel)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean selected; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetSelected(PRBool *aSelected)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLOptionElement::SetSelected(PRBool aSelected)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString value; */
NS_IMETHODIMP nsDOMHTMLOptionElement::GetValue(nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLOptionElement::SetValue(const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDOMHTMLParamElement */
#define NS_IDOMHTMLPARAMELEMENT_IID_STR "a6cf90ad-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLPARAMELEMENT_IID \
  {0xa6cf90ad, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLParamElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLPARAMELEMENT_IID)

  /* attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;
  NS_IMETHOD SetName(const nsAString & aName) = 0;

  /* attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;
  NS_IMETHOD SetType(const nsAString & aType) = 0;

  /* attribute DOMString value; */
  NS_IMETHOD GetValue(nsAString & aValue) = 0;
  NS_IMETHOD SetValue(const nsAString & aValue) = 0;

  /* attribute DOMString valueType; */
  NS_IMETHOD GetValueType(nsAString & aValueType) = 0;
  NS_IMETHOD SetValueType(const nsAString & aValueType) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLPARAMELEMENT \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD SetName(const nsAString & aName); \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD SetType(const nsAString & aType); \
  NS_IMETHOD GetValue(nsAString & aValue); \
  NS_IMETHOD SetValue(const nsAString & aValue); \
  NS_IMETHOD GetValueType(nsAString & aValueType); \
  NS_IMETHOD SetValueType(const nsAString & aValueType); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLPARAMELEMENT(_to) \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return _to SetName(aName); } \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return _to SetType(aType); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return _to GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return _to SetValue(aValue); } \
  NS_IMETHOD GetValueType(nsAString & aValueType) { return _to GetValueType(aValueType); } \
  NS_IMETHOD SetValueType(const nsAString & aValueType) { return _to SetValueType(aValueType); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLPARAMELEMENT(_to) \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD SetType(const nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetType(aType); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValue(aValue); } \
  NS_IMETHOD GetValueType(nsAString & aValueType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValueType(aValueType); } \
  NS_IMETHOD SetValueType(const nsAString & aValueType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValueType(aValueType); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLParamElement : public nsIDOMHTMLParamElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLPARAMELEMENT

  nsDOMHTMLParamElement();

private:
  ~nsDOMHTMLParamElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLParamElement, nsIDOMHTMLParamElement)

nsDOMHTMLParamElement::nsDOMHTMLParamElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLParamElement::~nsDOMHTMLParamElement()
{
  /* destructor code */
}

/* attribute DOMString name; */
NS_IMETHODIMP nsDOMHTMLParamElement::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLParamElement::SetName(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString type; */
NS_IMETHODIMP nsDOMHTMLParamElement::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLParamElement::SetType(const nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString value; */
NS_IMETHODIMP nsDOMHTMLParamElement::GetValue(nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLParamElement::SetValue(const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString valueType; */
NS_IMETHODIMP nsDOMHTMLParamElement::GetValueType(nsAString & aValueType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLParamElement::SetValueType(const nsAString & aValueType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMHTMLFormElement; /* forward declaration */

class nsIDOMHTMLOptionsCollection; /* forward declaration */


/* starting interface:    nsIDOMHTMLSelectElement */
#define NS_IDOMHTMLSELECTELEMENT_IID_STR "a6cf9090-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLSELECTELEMENT_IID \
  {0xa6cf9090, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLSelectElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLSELECTELEMENT_IID)

  /* readonly attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;

  /* attribute long selectedIndex; */
  NS_IMETHOD GetSelectedIndex(PRInt32 *aSelectedIndex) = 0;
  NS_IMETHOD SetSelectedIndex(PRInt32 aSelectedIndex) = 0;

  /* attribute DOMString value; */
  NS_IMETHOD GetValue(nsAString & aValue) = 0;
  NS_IMETHOD SetValue(const nsAString & aValue) = 0;

  /* attribute unsigned long length; */
  NS_IMETHOD GetLength(PRUint32 *aLength) = 0;
  NS_IMETHOD SetLength(PRUint32 aLength) = 0;

  /* readonly attribute nsIDOMHTMLFormElement form; */
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) = 0;

  /* readonly attribute nsIDOMHTMLOptionsCollection options; */
  NS_IMETHOD GetOptions(nsIDOMHTMLOptionsCollection * *aOptions) = 0;

  /* attribute boolean disabled; */
  NS_IMETHOD GetDisabled(PRBool *aDisabled) = 0;
  NS_IMETHOD SetDisabled(PRBool aDisabled) = 0;

  /* attribute boolean multiple; */
  NS_IMETHOD GetMultiple(PRBool *aMultiple) = 0;
  NS_IMETHOD SetMultiple(PRBool aMultiple) = 0;

  /* attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;
  NS_IMETHOD SetName(const nsAString & aName) = 0;

  /* attribute long size; */
  NS_IMETHOD GetSize(PRInt32 *aSize) = 0;
  NS_IMETHOD SetSize(PRInt32 aSize) = 0;

  /* attribute long tabIndex; */
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) = 0;
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) = 0;

  /* void add (in nsIDOMHTMLElement element, in nsIDOMHTMLElement before)  raises (DOMException); */
  NS_IMETHOD Add(nsIDOMHTMLElement *element, nsIDOMHTMLElement *before) = 0;

  /* void remove (in long index); */
  NS_IMETHOD Remove(PRInt32 index) = 0;

  /* void blur (); */
  NS_IMETHOD Blur(void) = 0;

  /* void focus (); */
  NS_IMETHOD Focus(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLSELECTELEMENT \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD GetSelectedIndex(PRInt32 *aSelectedIndex); \
  NS_IMETHOD SetSelectedIndex(PRInt32 aSelectedIndex); \
  NS_IMETHOD GetValue(nsAString & aValue); \
  NS_IMETHOD SetValue(const nsAString & aValue); \
  NS_IMETHOD GetLength(PRUint32 *aLength); \
  NS_IMETHOD SetLength(PRUint32 aLength); \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm); \
  NS_IMETHOD GetOptions(nsIDOMHTMLOptionsCollection * *aOptions); \
  NS_IMETHOD GetDisabled(PRBool *aDisabled); \
  NS_IMETHOD SetDisabled(PRBool aDisabled); \
  NS_IMETHOD GetMultiple(PRBool *aMultiple); \
  NS_IMETHOD SetMultiple(PRBool aMultiple); \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD SetName(const nsAString & aName); \
  NS_IMETHOD GetSize(PRInt32 *aSize); \
  NS_IMETHOD SetSize(PRInt32 aSize); \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex); \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex); \
  NS_IMETHOD Add(nsIDOMHTMLElement *element, nsIDOMHTMLElement *before); \
  NS_IMETHOD Remove(PRInt32 index); \
  NS_IMETHOD Blur(void); \
  NS_IMETHOD Focus(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLSELECTELEMENT(_to) \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD GetSelectedIndex(PRInt32 *aSelectedIndex) { return _to GetSelectedIndex(aSelectedIndex); } \
  NS_IMETHOD SetSelectedIndex(PRInt32 aSelectedIndex) { return _to SetSelectedIndex(aSelectedIndex); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return _to GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return _to SetValue(aValue); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return _to GetLength(aLength); } \
  NS_IMETHOD SetLength(PRUint32 aLength) { return _to SetLength(aLength); } \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return _to GetForm(aForm); } \
  NS_IMETHOD GetOptions(nsIDOMHTMLOptionsCollection * *aOptions) { return _to GetOptions(aOptions); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return _to GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return _to SetDisabled(aDisabled); } \
  NS_IMETHOD GetMultiple(PRBool *aMultiple) { return _to GetMultiple(aMultiple); } \
  NS_IMETHOD SetMultiple(PRBool aMultiple) { return _to SetMultiple(aMultiple); } \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return _to SetName(aName); } \
  NS_IMETHOD GetSize(PRInt32 *aSize) { return _to GetSize(aSize); } \
  NS_IMETHOD SetSize(PRInt32 aSize) { return _to SetSize(aSize); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return _to GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return _to SetTabIndex(aTabIndex); } \
  NS_IMETHOD Add(nsIDOMHTMLElement *element, nsIDOMHTMLElement *before) { return _to Add(element, before); } \
  NS_IMETHOD Remove(PRInt32 index) { return _to Remove(index); } \
  NS_IMETHOD Blur(void) { return _to Blur(); } \
  NS_IMETHOD Focus(void) { return _to Focus(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLSELECTELEMENT(_to) \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD GetSelectedIndex(PRInt32 *aSelectedIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSelectedIndex(aSelectedIndex); } \
  NS_IMETHOD SetSelectedIndex(PRInt32 aSelectedIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSelectedIndex(aSelectedIndex); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValue(aValue); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLength(aLength); } \
  NS_IMETHOD SetLength(PRUint32 aLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLength(aLength); } \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetForm(aForm); } \
  NS_IMETHOD GetOptions(nsIDOMHTMLOptionsCollection * *aOptions) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOptions(aOptions); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDisabled(aDisabled); } \
  NS_IMETHOD GetMultiple(PRBool *aMultiple) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMultiple(aMultiple); } \
  NS_IMETHOD SetMultiple(PRBool aMultiple) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMultiple(aMultiple); } \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD GetSize(PRInt32 *aSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSize(aSize); } \
  NS_IMETHOD SetSize(PRInt32 aSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSize(aSize); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabIndex(aTabIndex); } \
  NS_IMETHOD Add(nsIDOMHTMLElement *element, nsIDOMHTMLElement *before) { return !_to ? NS_ERROR_NULL_POINTER : _to->Add(element, before); } \
  NS_IMETHOD Remove(PRInt32 index) { return !_to ? NS_ERROR_NULL_POINTER : _to->Remove(index); } \
  NS_IMETHOD Blur(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Blur(); } \
  NS_IMETHOD Focus(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Focus(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLSelectElement : public nsIDOMHTMLSelectElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLSELECTELEMENT

  nsDOMHTMLSelectElement();

private:
  ~nsDOMHTMLSelectElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLSelectElement, nsIDOMHTMLSelectElement)

nsDOMHTMLSelectElement::nsDOMHTMLSelectElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLSelectElement::~nsDOMHTMLSelectElement()
{
  /* destructor code */
}

/* readonly attribute DOMString type; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long selectedIndex; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetSelectedIndex(PRInt32 *aSelectedIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetSelectedIndex(PRInt32 aSelectedIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString value; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetValue(nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetValue(const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute unsigned long length; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetLength(PRUint32 *aLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetLength(PRUint32 aLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMHTMLFormElement form; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetForm(nsIDOMHTMLFormElement * *aForm)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMHTMLOptionsCollection options; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetOptions(nsIDOMHTMLOptionsCollection * *aOptions)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean disabled; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetDisabled(PRBool *aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetDisabled(PRBool aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean multiple; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetMultiple(PRBool *aMultiple)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetMultiple(PRBool aMultiple)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString name; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetName(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long size; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetSize(PRInt32 *aSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetSize(PRInt32 aSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long tabIndex; */
NS_IMETHODIMP nsDOMHTMLSelectElement::GetTabIndex(PRInt32 *aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLSelectElement::SetTabIndex(PRInt32 aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void add (in nsIDOMHTMLElement element, in nsIDOMHTMLElement before)  raises (DOMException); */
NS_IMETHODIMP nsDOMHTMLSelectElement::Add(nsIDOMHTMLElement *element, nsIDOMHTMLElement *before)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void remove (in long index); */
NS_IMETHODIMP nsDOMHTMLSelectElement::Remove(PRInt32 index)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void blur (); */
NS_IMETHODIMP nsDOMHTMLSelectElement::Blur()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void focus (); */
NS_IMETHODIMP nsDOMHTMLSelectElement::Focus()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMHTMLFormElement; /* forward declaration */


/* starting interface:    nsIDOMHTMLTextAreaElement */
#define NS_IDOMHTMLTEXTAREAELEMENT_IID_STR "a6cf9094-15b3-11d2-932e-00805f8add32"

#define NS_IDOMHTMLTEXTAREAELEMENT_IID \
  {0xa6cf9094, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMHTMLTextAreaElement : public nsIDOMHTMLElement {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMHTMLTEXTAREAELEMENT_IID)

  /* attribute DOMString defaultValue; */
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue) = 0;
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue) = 0;

  /* readonly attribute nsIDOMHTMLFormElement form; */
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) = 0;

  /* attribute DOMString accessKey; */
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) = 0;
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) = 0;

  /* attribute long cols; */
  NS_IMETHOD GetCols(PRInt32 *aCols) = 0;
  NS_IMETHOD SetCols(PRInt32 aCols) = 0;

  /* attribute boolean disabled; */
  NS_IMETHOD GetDisabled(PRBool *aDisabled) = 0;
  NS_IMETHOD SetDisabled(PRBool aDisabled) = 0;

  /* attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;
  NS_IMETHOD SetName(const nsAString & aName) = 0;

  /* attribute boolean readOnly; */
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly) = 0;
  NS_IMETHOD SetReadOnly(PRBool aReadOnly) = 0;

  /* attribute long rows; */
  NS_IMETHOD GetRows(PRInt32 *aRows) = 0;
  NS_IMETHOD SetRows(PRInt32 aRows) = 0;

  /* attribute long tabIndex; */
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) = 0;
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) = 0;

  /* readonly attribute DOMString type; */
  NS_IMETHOD GetType(nsAString & aType) = 0;

  /* attribute DOMString value; */
  NS_IMETHOD GetValue(nsAString & aValue) = 0;
  NS_IMETHOD SetValue(const nsAString & aValue) = 0;

  /* void blur (); */
  NS_IMETHOD Blur(void) = 0;

  /* void focus (); */
  NS_IMETHOD Focus(void) = 0;

  /* void select (); */
  NS_IMETHOD Select(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMHTMLTEXTAREAELEMENT \
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue); \
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue); \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm); \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey); \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey); \
  NS_IMETHOD GetCols(PRInt32 *aCols); \
  NS_IMETHOD SetCols(PRInt32 aCols); \
  NS_IMETHOD GetDisabled(PRBool *aDisabled); \
  NS_IMETHOD SetDisabled(PRBool aDisabled); \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD SetName(const nsAString & aName); \
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly); \
  NS_IMETHOD SetReadOnly(PRBool aReadOnly); \
  NS_IMETHOD GetRows(PRInt32 *aRows); \
  NS_IMETHOD SetRows(PRInt32 aRows); \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex); \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex); \
  NS_IMETHOD GetType(nsAString & aType); \
  NS_IMETHOD GetValue(nsAString & aValue); \
  NS_IMETHOD SetValue(const nsAString & aValue); \
  NS_IMETHOD Blur(void); \
  NS_IMETHOD Focus(void); \
  NS_IMETHOD Select(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMHTMLTEXTAREAELEMENT(_to) \
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue) { return _to GetDefaultValue(aDefaultValue); } \
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue) { return _to SetDefaultValue(aDefaultValue); } \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return _to GetForm(aForm); } \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return _to GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return _to SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetCols(PRInt32 *aCols) { return _to GetCols(aCols); } \
  NS_IMETHOD SetCols(PRInt32 aCols) { return _to SetCols(aCols); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return _to GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return _to SetDisabled(aDisabled); } \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return _to SetName(aName); } \
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly) { return _to GetReadOnly(aReadOnly); } \
  NS_IMETHOD SetReadOnly(PRBool aReadOnly) { return _to SetReadOnly(aReadOnly); } \
  NS_IMETHOD GetRows(PRInt32 *aRows) { return _to GetRows(aRows); } \
  NS_IMETHOD SetRows(PRInt32 aRows) { return _to SetRows(aRows); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return _to GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return _to SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetType(nsAString & aType) { return _to GetType(aType); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return _to GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return _to SetValue(aValue); } \
  NS_IMETHOD Blur(void) { return _to Blur(); } \
  NS_IMETHOD Focus(void) { return _to Focus(); } \
  NS_IMETHOD Select(void) { return _to Select(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMHTMLTEXTAREAELEMENT(_to) \
  NS_IMETHOD GetDefaultValue(nsAString & aDefaultValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultValue(aDefaultValue); } \
  NS_IMETHOD SetDefaultValue(const nsAString & aDefaultValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultValue(aDefaultValue); } \
  NS_IMETHOD GetForm(nsIDOMHTMLFormElement * *aForm) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetForm(aForm); } \
  NS_IMETHOD GetAccessKey(nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAccessKey(aAccessKey); } \
  NS_IMETHOD SetAccessKey(const nsAString & aAccessKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAccessKey(aAccessKey); } \
  NS_IMETHOD GetCols(PRInt32 *aCols) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCols(aCols); } \
  NS_IMETHOD SetCols(PRInt32 aCols) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetCols(aCols); } \
  NS_IMETHOD GetDisabled(PRBool *aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDisabled(aDisabled); } \
  NS_IMETHOD SetDisabled(PRBool aDisabled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDisabled(aDisabled); } \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD GetReadOnly(PRBool *aReadOnly) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetReadOnly(aReadOnly); } \
  NS_IMETHOD SetReadOnly(PRBool aReadOnly) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetReadOnly(aReadOnly); } \
  NS_IMETHOD GetRows(PRInt32 *aRows) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRows(aRows); } \
  NS_IMETHOD SetRows(PRInt32 aRows) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetRows(aRows); } \
  NS_IMETHOD GetTabIndex(PRInt32 *aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTabIndex(aTabIndex); } \
  NS_IMETHOD SetTabIndex(PRInt32 aTabIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTabIndex(aTabIndex); } \
  NS_IMETHOD GetType(nsAString & aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetType(aType); } \
  NS_IMETHOD GetValue(nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetValue(aValue); } \
  NS_IMETHOD SetValue(const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetValue(aValue); } \
  NS_IMETHOD Blur(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Blur(); } \
  NS_IMETHOD Focus(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Focus(); } \
  NS_IMETHOD Select(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Select(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMHTMLTextAreaElement : public nsIDOMHTMLTextAreaElement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMHTMLTEXTAREAELEMENT

  nsDOMHTMLTextAreaElement();

private:
  ~nsDOMHTMLTextAreaElement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMHTMLTextAreaElement, nsIDOMHTMLTextAreaElement)

nsDOMHTMLTextAreaElement::nsDOMHTMLTextAreaElement()
{
  /* member initializers and constructor code */
}

nsDOMHTMLTextAreaElement::~nsDOMHTMLTextAreaElement()
{
  /* destructor code */
}

/* attribute DOMString defaultValue; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetDefaultValue(nsAString & aDefaultValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetDefaultValue(const nsAString & aDefaultValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMHTMLFormElement form; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetForm(nsIDOMHTMLFormElement * *aForm)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString accessKey; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetAccessKey(nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetAccessKey(const nsAString & aAccessKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long cols; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetCols(PRInt32 *aCols)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetCols(PRInt32 aCols)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean disabled; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetDisabled(PRBool *aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetDisabled(PRBool aDisabled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString name; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetName(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean readOnly; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetReadOnly(PRBool *aReadOnly)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetReadOnly(PRBool aReadOnly)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long rows; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetRows(PRInt32 *aRows)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetRows(PRInt32 aRows)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long tabIndex; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetTabIndex(PRInt32 *aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetTabIndex(PRInt32 aTabIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute DOMString type; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetType(nsAString & aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString value; */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::GetValue(nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMHTMLTextAreaElement::SetValue(const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void blur (); */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::Blur()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void focus (); */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::Focus()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void select (); */
NS_IMETHODIMP nsDOMHTMLTextAreaElement::Select()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMAbstractView; /* forward declaration */


/* starting interface:    nsIDOMUIEvent */
#define NS_IDOMUIEVENT_IID_STR "a6cf90c3-15b3-11d2-932e-00805f8add32"

#define NS_IDOMUIEVENT_IID \
  {0xa6cf90c3, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMUIEvent : public nsIDOMEvent {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMUIEVENT_IID)

  /* readonly attribute nsIDOMAbstractView view; */
  NS_IMETHOD GetView(nsIDOMAbstractView * *aView) = 0;

  /* readonly attribute long detail; */
  NS_IMETHOD GetDetail(PRInt32 *aDetail) = 0;

  /* void initUIEvent (in DOMString type_arg, in boolean can_bubble_arg, in boolean cancelable_arg, in nsIDOMAbstractView view_arg, in long detail_arg); */
  NS_IMETHOD InitUIEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMUIEVENT \
  NS_IMETHOD GetView(nsIDOMAbstractView * *aView); \
  NS_IMETHOD GetDetail(PRInt32 *aDetail); \
  NS_IMETHOD InitUIEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMUIEVENT(_to) \
  NS_IMETHOD GetView(nsIDOMAbstractView * *aView) { return _to GetView(aView); } \
  NS_IMETHOD GetDetail(PRInt32 *aDetail) { return _to GetDetail(aDetail); } \
  NS_IMETHOD InitUIEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg) { return _to InitUIEvent(type_arg, can_bubble_arg, cancelable_arg, view_arg, detail_arg); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMUIEVENT(_to) \
  NS_IMETHOD GetView(nsIDOMAbstractView * *aView) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetView(aView); } \
  NS_IMETHOD GetDetail(PRInt32 *aDetail) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDetail(aDetail); } \
  NS_IMETHOD InitUIEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg) { return !_to ? NS_ERROR_NULL_POINTER : _to->InitUIEvent(type_arg, can_bubble_arg, cancelable_arg, view_arg, detail_arg); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMUIEvent : public nsIDOMUIEvent
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMUIEVENT

  nsDOMUIEvent();

private:
  ~nsDOMUIEvent();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMUIEvent, nsIDOMUIEvent)

nsDOMUIEvent::nsDOMUIEvent()
{
  /* member initializers and constructor code */
}

nsDOMUIEvent::~nsDOMUIEvent()
{
  /* destructor code */
}

/* readonly attribute nsIDOMAbstractView view; */
NS_IMETHODIMP nsDOMUIEvent::GetView(nsIDOMAbstractView * *aView)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long detail; */
NS_IMETHODIMP nsDOMUIEvent::GetDetail(PRInt32 *aDetail)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void initUIEvent (in DOMString type_arg, in boolean can_bubble_arg, in boolean cancelable_arg, in nsIDOMAbstractView view_arg, in long detail_arg); */
NS_IMETHODIMP nsDOMUIEvent::InitUIEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMAbstractView; /* forward declaration */

class nsIDOMEventTarget; /* forward declaration */


/* starting interface:    nsIDOMMouseEvent */
#define NS_IDOMMOUSEEVENT_IID_STR "ff751edc-8b02-aae7-0010-8301838a3123"

#define NS_IDOMMOUSEEVENT_IID \
  {0xff751edc, 0x8b02, 0xaae7, \
    { 0x00, 0x10, 0x83, 0x01, 0x83, 0x8a, 0x31, 0x23 }}

class NS_NO_VTABLE nsIDOMMouseEvent : public nsIDOMUIEvent {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMMOUSEEVENT_IID)

  /* readonly attribute long screenX; */
  NS_IMETHOD GetScreenX(PRInt32 *aScreenX) = 0;

  /* readonly attribute long screenY; */
  NS_IMETHOD GetScreenY(PRInt32 *aScreenY) = 0;

  /* readonly attribute long clientX; */
  NS_IMETHOD GetClientX(PRInt32 *aClientX) = 0;

  /* readonly attribute long clientY; */
  NS_IMETHOD GetClientY(PRInt32 *aClientY) = 0;

  /* readonly attribute boolean ctrlKey; */
  NS_IMETHOD GetCtrlKey(PRBool *aCtrlKey) = 0;

  /* readonly attribute boolean shiftKey; */
  NS_IMETHOD GetShiftKey(PRBool *aShiftKey) = 0;

  /* readonly attribute boolean altKey; */
  NS_IMETHOD GetAltKey(PRBool *aAltKey) = 0;

  /* readonly attribute boolean metaKey; */
  NS_IMETHOD GetMetaKey(PRBool *aMetaKey) = 0;

  /* readonly attribute unsigned short button; */
  NS_IMETHOD GetButton(PRUint16 *aButton) = 0;

  /* readonly attribute nsIDOMEventTarget relatedTarget; */
  NS_IMETHOD GetRelatedTarget(nsIDOMEventTarget * *aRelatedTarget) = 0;

  /* void initMouseEvent (in DOMString type_arg, in boolean can_bubble_arg, in boolean cancelable_arg, in nsIDOMAbstractView view_arg, in long detail_arg, in long screen_x_arg, in long screen_y_arg, in long client_x_arg, in long client_y_arg, in boolean ctrl_key_arg, in boolean alt_key_arg, in boolean shift_key_arg, in boolean meta_key_arg, in unsigned short button_arg, in nsIDOMEventTarget related_target_arg); */
  NS_IMETHOD InitMouseEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg, PRInt32 screen_x_arg, PRInt32 screen_y_arg, PRInt32 client_x_arg, PRInt32 client_y_arg, PRBool ctrl_key_arg, PRBool alt_key_arg, PRBool shift_key_arg, PRBool meta_key_arg, PRUint16 button_arg, nsIDOMEventTarget *related_target_arg) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMMOUSEEVENT \
  NS_IMETHOD GetScreenX(PRInt32 *aScreenX); \
  NS_IMETHOD GetScreenY(PRInt32 *aScreenY); \
  NS_IMETHOD GetClientX(PRInt32 *aClientX); \
  NS_IMETHOD GetClientY(PRInt32 *aClientY); \
  NS_IMETHOD GetCtrlKey(PRBool *aCtrlKey); \
  NS_IMETHOD GetShiftKey(PRBool *aShiftKey); \
  NS_IMETHOD GetAltKey(PRBool *aAltKey); \
  NS_IMETHOD GetMetaKey(PRBool *aMetaKey); \
  NS_IMETHOD GetButton(PRUint16 *aButton); \
  NS_IMETHOD GetRelatedTarget(nsIDOMEventTarget * *aRelatedTarget); \
  NS_IMETHOD InitMouseEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg, PRInt32 screen_x_arg, PRInt32 screen_y_arg, PRInt32 client_x_arg, PRInt32 client_y_arg, PRBool ctrl_key_arg, PRBool alt_key_arg, PRBool shift_key_arg, PRBool meta_key_arg, PRUint16 button_arg, nsIDOMEventTarget *related_target_arg); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMMOUSEEVENT(_to) \
  NS_IMETHOD GetScreenX(PRInt32 *aScreenX) { return _to GetScreenX(aScreenX); } \
  NS_IMETHOD GetScreenY(PRInt32 *aScreenY) { return _to GetScreenY(aScreenY); } \
  NS_IMETHOD GetClientX(PRInt32 *aClientX) { return _to GetClientX(aClientX); } \
  NS_IMETHOD GetClientY(PRInt32 *aClientY) { return _to GetClientY(aClientY); } \
  NS_IMETHOD GetCtrlKey(PRBool *aCtrlKey) { return _to GetCtrlKey(aCtrlKey); } \
  NS_IMETHOD GetShiftKey(PRBool *aShiftKey) { return _to GetShiftKey(aShiftKey); } \
  NS_IMETHOD GetAltKey(PRBool *aAltKey) { return _to GetAltKey(aAltKey); } \
  NS_IMETHOD GetMetaKey(PRBool *aMetaKey) { return _to GetMetaKey(aMetaKey); } \
  NS_IMETHOD GetButton(PRUint16 *aButton) { return _to GetButton(aButton); } \
  NS_IMETHOD GetRelatedTarget(nsIDOMEventTarget * *aRelatedTarget) { return _to GetRelatedTarget(aRelatedTarget); } \
  NS_IMETHOD InitMouseEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg, PRInt32 screen_x_arg, PRInt32 screen_y_arg, PRInt32 client_x_arg, PRInt32 client_y_arg, PRBool ctrl_key_arg, PRBool alt_key_arg, PRBool shift_key_arg, PRBool meta_key_arg, PRUint16 button_arg, nsIDOMEventTarget *related_target_arg) { return _to InitMouseEvent(type_arg, can_bubble_arg, cancelable_arg, view_arg, detail_arg, screen_x_arg, screen_y_arg, client_x_arg, client_y_arg, ctrl_key_arg, alt_key_arg, shift_key_arg, meta_key_arg, button_arg, related_target_arg); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMMOUSEEVENT(_to) \
  NS_IMETHOD GetScreenX(PRInt32 *aScreenX) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScreenX(aScreenX); } \
  NS_IMETHOD GetScreenY(PRInt32 *aScreenY) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScreenY(aScreenY); } \
  NS_IMETHOD GetClientX(PRInt32 *aClientX) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetClientX(aClientX); } \
  NS_IMETHOD GetClientY(PRInt32 *aClientY) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetClientY(aClientY); } \
  NS_IMETHOD GetCtrlKey(PRBool *aCtrlKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCtrlKey(aCtrlKey); } \
  NS_IMETHOD GetShiftKey(PRBool *aShiftKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetShiftKey(aShiftKey); } \
  NS_IMETHOD GetAltKey(PRBool *aAltKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAltKey(aAltKey); } \
  NS_IMETHOD GetMetaKey(PRBool *aMetaKey) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMetaKey(aMetaKey); } \
  NS_IMETHOD GetButton(PRUint16 *aButton) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetButton(aButton); } \
  NS_IMETHOD GetRelatedTarget(nsIDOMEventTarget * *aRelatedTarget) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRelatedTarget(aRelatedTarget); } \
  NS_IMETHOD InitMouseEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg, PRInt32 screen_x_arg, PRInt32 screen_y_arg, PRInt32 client_x_arg, PRInt32 client_y_arg, PRBool ctrl_key_arg, PRBool alt_key_arg, PRBool shift_key_arg, PRBool meta_key_arg, PRUint16 button_arg, nsIDOMEventTarget *related_target_arg) { return !_to ? NS_ERROR_NULL_POINTER : _to->InitMouseEvent(type_arg, can_bubble_arg, cancelable_arg, view_arg, detail_arg, screen_x_arg, screen_y_arg, client_x_arg, client_y_arg, ctrl_key_arg, alt_key_arg, shift_key_arg, meta_key_arg, button_arg, related_target_arg); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMMouseEvent : public nsIDOMMouseEvent
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMMOUSEEVENT

  nsDOMMouseEvent();

private:
  ~nsDOMMouseEvent();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMMouseEvent, nsIDOMMouseEvent)

nsDOMMouseEvent::nsDOMMouseEvent()
{
  /* member initializers and constructor code */
}

nsDOMMouseEvent::~nsDOMMouseEvent()
{
  /* destructor code */
}

/* readonly attribute long screenX; */
NS_IMETHODIMP nsDOMMouseEvent::GetScreenX(PRInt32 *aScreenX)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long screenY; */
NS_IMETHODIMP nsDOMMouseEvent::GetScreenY(PRInt32 *aScreenY)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long clientX; */
NS_IMETHODIMP nsDOMMouseEvent::GetClientX(PRInt32 *aClientX)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long clientY; */
NS_IMETHODIMP nsDOMMouseEvent::GetClientY(PRInt32 *aClientY)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean ctrlKey; */
NS_IMETHODIMP nsDOMMouseEvent::GetCtrlKey(PRBool *aCtrlKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean shiftKey; */
NS_IMETHODIMP nsDOMMouseEvent::GetShiftKey(PRBool *aShiftKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean altKey; */
NS_IMETHODIMP nsDOMMouseEvent::GetAltKey(PRBool *aAltKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean metaKey; */
NS_IMETHODIMP nsDOMMouseEvent::GetMetaKey(PRBool *aMetaKey)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned short button; */
NS_IMETHODIMP nsDOMMouseEvent::GetButton(PRUint16 *aButton)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMEventTarget relatedTarget; */
NS_IMETHODIMP nsDOMMouseEvent::GetRelatedTarget(nsIDOMEventTarget * *aRelatedTarget)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void initMouseEvent (in DOMString type_arg, in boolean can_bubble_arg, in boolean cancelable_arg, in nsIDOMAbstractView view_arg, in long detail_arg, in long screen_x_arg, in long screen_y_arg, in long client_x_arg, in long client_y_arg, in boolean ctrl_key_arg, in boolean alt_key_arg, in boolean shift_key_arg, in boolean meta_key_arg, in unsigned short button_arg, in nsIDOMEventTarget related_target_arg); */
NS_IMETHODIMP nsDOMMouseEvent::InitMouseEvent(const nsAString & type_arg, PRBool can_bubble_arg, PRBool cancelable_arg, nsIDOMAbstractView *view_arg, PRInt32 detail_arg, PRInt32 screen_x_arg, PRInt32 screen_y_arg, PRInt32 client_x_arg, PRInt32 client_y_arg, PRBool ctrl_key_arg, PRBool alt_key_arg, PRBool shift_key_arg, PRBool meta_key_arg, PRUint16 button_arg, nsIDOMEventTarget *related_target_arg)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMNode; /* forward declaration */

class DOMException; /* forward declaration */


/* starting interface:    nsIDOMNamedNodeMap */
#define NS_IDOMNAMEDNODEMAP_IID_STR "a6cf907b-15b3-11d2-932e-00805f8add32"

#define NS_IDOMNAMEDNODEMAP_IID \
  {0xa6cf907b, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMNamedNodeMap : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMNAMEDNODEMAP_IID)

  /* nsIDOMNode getNamedItem (in DOMString name); */
  NS_IMETHOD GetNamedItem(const nsAString & name, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode setNamedItem (in nsIDOMNode arg)  raises (DOMException); */
  NS_IMETHOD SetNamedItem(nsIDOMNode *arg, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode removeNamedItem (in DOMString name)  raises (DOMException); */
  NS_IMETHOD RemoveNamedItem(const nsAString & name, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode item (in unsigned long index); */
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval) = 0;

  /* readonly attribute unsigned long length; */
  NS_IMETHOD GetLength(PRUint32 *aLength) = 0;

  /* nsIDOMNode getNamedItemNS (in DOMString namespace_uri, in DOMString local_name); */
  NS_IMETHOD GetNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode setNamedItemNS (in nsIDOMNode arg)  raises (DOMException); */
  NS_IMETHOD SetNamedItemNS(nsIDOMNode *arg, nsIDOMNode **_retval) = 0;

  /* nsIDOMNode removeNamedItemNS (in DOMString namespace_uri, in DOMString local_name)  raises (DOMException); */
  NS_IMETHOD RemoveNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMNAMEDNODEMAP \
  NS_IMETHOD GetNamedItem(const nsAString & name, nsIDOMNode **_retval); \
  NS_IMETHOD SetNamedItem(nsIDOMNode *arg, nsIDOMNode **_retval); \
  NS_IMETHOD RemoveNamedItem(const nsAString & name, nsIDOMNode **_retval); \
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval); \
  NS_IMETHOD GetLength(PRUint32 *aLength); \
  NS_IMETHOD GetNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval); \
  NS_IMETHOD SetNamedItemNS(nsIDOMNode *arg, nsIDOMNode **_retval); \
  NS_IMETHOD RemoveNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMNAMEDNODEMAP(_to) \
  NS_IMETHOD GetNamedItem(const nsAString & name, nsIDOMNode **_retval) { return _to GetNamedItem(name, _retval); } \
  NS_IMETHOD SetNamedItem(nsIDOMNode *arg, nsIDOMNode **_retval) { return _to SetNamedItem(arg, _retval); } \
  NS_IMETHOD RemoveNamedItem(const nsAString & name, nsIDOMNode **_retval) { return _to RemoveNamedItem(name, _retval); } \
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval) { return _to Item(index, _retval); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return _to GetLength(aLength); } \
  NS_IMETHOD GetNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval) { return _to GetNamedItemNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD SetNamedItemNS(nsIDOMNode *arg, nsIDOMNode **_retval) { return _to SetNamedItemNS(arg, _retval); } \
  NS_IMETHOD RemoveNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval) { return _to RemoveNamedItemNS(namespace_uri, local_name, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMNAMEDNODEMAP(_to) \
  NS_IMETHOD GetNamedItem(const nsAString & name, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNamedItem(name, _retval); } \
  NS_IMETHOD SetNamedItem(nsIDOMNode *arg, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetNamedItem(arg, _retval); } \
  NS_IMETHOD RemoveNamedItem(const nsAString & name, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveNamedItem(name, _retval); } \
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Item(index, _retval); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLength(aLength); } \
  NS_IMETHOD GetNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNamedItemNS(namespace_uri, local_name, _retval); } \
  NS_IMETHOD SetNamedItemNS(nsIDOMNode *arg, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetNamedItemNS(arg, _retval); } \
  NS_IMETHOD RemoveNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveNamedItemNS(namespace_uri, local_name, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMNamedNodeMap : public nsIDOMNamedNodeMap
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMNAMEDNODEMAP

  nsDOMNamedNodeMap();

private:
  ~nsDOMNamedNodeMap();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMNamedNodeMap, nsIDOMNamedNodeMap)

nsDOMNamedNodeMap::nsDOMNamedNodeMap()
{
  /* member initializers and constructor code */
}

nsDOMNamedNodeMap::~nsDOMNamedNodeMap()
{
  /* destructor code */
}

/* nsIDOMNode getNamedItem (in DOMString name); */
NS_IMETHODIMP nsDOMNamedNodeMap::GetNamedItem(const nsAString & name, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode setNamedItem (in nsIDOMNode arg)  raises (DOMException); */
NS_IMETHODIMP nsDOMNamedNodeMap::SetNamedItem(nsIDOMNode *arg, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode removeNamedItem (in DOMString name)  raises (DOMException); */
NS_IMETHODIMP nsDOMNamedNodeMap::RemoveNamedItem(const nsAString & name, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode item (in unsigned long index); */
NS_IMETHODIMP nsDOMNamedNodeMap::Item(PRUint32 index, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned long length; */
NS_IMETHODIMP nsDOMNamedNodeMap::GetLength(PRUint32 *aLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode getNamedItemNS (in DOMString namespace_uri, in DOMString local_name); */
NS_IMETHODIMP nsDOMNamedNodeMap::GetNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode setNamedItemNS (in nsIDOMNode arg)  raises (DOMException); */
NS_IMETHODIMP nsDOMNamedNodeMap::SetNamedItemNS(nsIDOMNode *arg, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMNode removeNamedItemNS (in DOMString namespace_uri, in DOMString local_name)  raises (DOMException); */
NS_IMETHODIMP nsDOMNamedNodeMap::RemoveNamedItemNS(const nsAString & namespace_uri, const nsAString & local_name, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMNode; /* forward declaration */


/* starting interface:    nsIDOMNodeList */
#define NS_IDOMNODELIST_IID_STR "a6cf907d-15b3-11d2-932e-00805f8add32"

#define NS_IDOMNODELIST_IID \
  {0xa6cf907d, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMNodeList : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMNODELIST_IID)

  /* nsIDOMNode item (in unsigned long index); */
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval) = 0;

  /* readonly attribute unsigned long length; */
  NS_IMETHOD GetLength(PRUint32 *aLength) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMNODELIST \
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval); \
  NS_IMETHOD GetLength(PRUint32 *aLength); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMNODELIST(_to) \
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval) { return _to Item(index, _retval); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return _to GetLength(aLength); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMNODELIST(_to) \
  NS_IMETHOD Item(PRUint32 index, nsIDOMNode **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Item(index, _retval); } \
  NS_IMETHOD GetLength(PRUint32 *aLength) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLength(aLength); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMNodeList : public nsIDOMNodeList
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMNODELIST

  nsDOMNodeList();

private:
  ~nsDOMNodeList();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMNodeList, nsIDOMNodeList)

nsDOMNodeList::nsDOMNodeList()
{
  /* member initializers and constructor code */
}

nsDOMNodeList::~nsDOMNodeList()
{
  /* destructor code */
}

/* nsIDOMNode item (in unsigned long index); */
NS_IMETHODIMP nsDOMNodeList::Item(PRUint32 index, nsIDOMNode **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned long length; */
NS_IMETHODIMP nsDOMNodeList::GetLength(PRUint32 *aLength)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDOMText */
#define NS_IDOMTEXT_IID_STR "a6cf9082-15b3-11d2-932e-00805f8add32"

#define NS_IDOMTEXT_IID \
  {0xa6cf9082, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMText : public nsIDOMCharacterData {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMTEXT_IID)

  /* nsIDOMText splitText (in unsigned long offset)  raises (DOMException); */
  NS_IMETHOD SplitText(PRUint32 offset, nsIDOMText **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMTEXT \
  NS_IMETHOD SplitText(PRUint32 offset, nsIDOMText **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMTEXT(_to) \
  NS_IMETHOD SplitText(PRUint32 offset, nsIDOMText **_retval) { return _to SplitText(offset, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMTEXT(_to) \
  NS_IMETHOD SplitText(PRUint32 offset, nsIDOMText **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SplitText(offset, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMText : public nsIDOMText
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMTEXT

  nsDOMText();

private:
  ~nsDOMText();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMText, nsIDOMText)

nsDOMText::nsDOMText()
{
  /* member initializers and constructor code */
}

nsDOMText::~nsDOMText()
{
  /* destructor code */
}

/* nsIDOMText splitText (in unsigned long offset)  raises (DOMException); */
NS_IMETHODIMP nsDOMText::SplitText(PRUint32 offset, nsIDOMText **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMDocument; /* forward declaration */

class nsIDOMBarProp; /* forward declaration */

class nsIDOMWindowCollection; /* forward declaration */

class nsISelection; /* forward declaration */


/* starting interface:    nsIDOMWindow */
#define NS_IDOMWINDOW_IID_STR "a6cf906b-15b3-11d2-932e-00805f8add32"

#define NS_IDOMWINDOW_IID \
  {0xa6cf906b, 0x15b3, 0x11d2, \
    { 0x93, 0x2e, 0x00, 0x80, 0x5f, 0x8a, 0xdd, 0x32 }}

class NS_NO_VTABLE nsIDOMWindow : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMWINDOW_IID)

  /* readonly attribute nsIDOMDocument document; */
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument) = 0;

  /* readonly attribute nsIDOMWindow parent; */
  NS_IMETHOD GetParent(nsIDOMWindow * *aParent) = 0;

  /* readonly attribute nsIDOMWindow top; */
  NS_IMETHOD GetTop(nsIDOMWindow * *aTop) = 0;

  /* readonly attribute nsIDOMBarProp scrollbars; */
  NS_IMETHOD GetScrollbars(nsIDOMBarProp * *aScrollbars) = 0;

  /* [noscript] readonly attribute nsIDOMWindowCollection frames; */
  NS_IMETHOD GetFrames(nsIDOMWindowCollection * *aFrames) = 0;

  /* attribute DOMString name; */
  NS_IMETHOD GetName(nsAString & aName) = 0;
  NS_IMETHOD SetName(const nsAString & aName) = 0;

  /* [noscript] attribute float textZoom; */
  NS_IMETHOD GetTextZoom(float *aTextZoom) = 0;
  NS_IMETHOD SetTextZoom(float aTextZoom) = 0;

  /* readonly attribute long scrollX; */
  NS_IMETHOD GetScrollX(PRInt32 *aScrollX) = 0;

  /* readonly attribute long scrollY; */
  NS_IMETHOD GetScrollY(PRInt32 *aScrollY) = 0;

  /* void scrollTo (in long x_scroll, in long y_scroll); */
  NS_IMETHOD ScrollTo(PRInt32 x_scroll, PRInt32 y_scroll) = 0;

  /* void scrollBy (in long x_scroll_diff, in long y_scroll_diff); */
  NS_IMETHOD ScrollBy(PRInt32 x_scroll_diff, PRInt32 y_scroll_diff) = 0;

  /* nsISelection getSelection (); */
  NS_IMETHOD GetSelection(nsISelection **_retval) = 0;

  /* void scrollByLines (in long num_lines); */
  NS_IMETHOD ScrollByLines(PRInt32 num_lines) = 0;

  /* void scrollByPages (in long num_pages); */
  NS_IMETHOD ScrollByPages(PRInt32 num_pages) = 0;

  /* void sizeToContent (); */
  NS_IMETHOD SizeToContent(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMWINDOW \
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument); \
  NS_IMETHOD GetParent(nsIDOMWindow * *aParent); \
  NS_IMETHOD GetTop(nsIDOMWindow * *aTop); \
  NS_IMETHOD GetScrollbars(nsIDOMBarProp * *aScrollbars); \
  NS_IMETHOD GetFrames(nsIDOMWindowCollection * *aFrames); \
  NS_IMETHOD GetName(nsAString & aName); \
  NS_IMETHOD SetName(const nsAString & aName); \
  NS_IMETHOD GetTextZoom(float *aTextZoom); \
  NS_IMETHOD SetTextZoom(float aTextZoom); \
  NS_IMETHOD GetScrollX(PRInt32 *aScrollX); \
  NS_IMETHOD GetScrollY(PRInt32 *aScrollY); \
  NS_IMETHOD ScrollTo(PRInt32 x_scroll, PRInt32 y_scroll); \
  NS_IMETHOD ScrollBy(PRInt32 x_scroll_diff, PRInt32 y_scroll_diff); \
  NS_IMETHOD GetSelection(nsISelection **_retval); \
  NS_IMETHOD ScrollByLines(PRInt32 num_lines); \
  NS_IMETHOD ScrollByPages(PRInt32 num_pages); \
  NS_IMETHOD SizeToContent(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMWINDOW(_to) \
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument) { return _to GetDocument(aDocument); } \
  NS_IMETHOD GetParent(nsIDOMWindow * *aParent) { return _to GetParent(aParent); } \
  NS_IMETHOD GetTop(nsIDOMWindow * *aTop) { return _to GetTop(aTop); } \
  NS_IMETHOD GetScrollbars(nsIDOMBarProp * *aScrollbars) { return _to GetScrollbars(aScrollbars); } \
  NS_IMETHOD GetFrames(nsIDOMWindowCollection * *aFrames) { return _to GetFrames(aFrames); } \
  NS_IMETHOD GetName(nsAString & aName) { return _to GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return _to SetName(aName); } \
  NS_IMETHOD GetTextZoom(float *aTextZoom) { return _to GetTextZoom(aTextZoom); } \
  NS_IMETHOD SetTextZoom(float aTextZoom) { return _to SetTextZoom(aTextZoom); } \
  NS_IMETHOD GetScrollX(PRInt32 *aScrollX) { return _to GetScrollX(aScrollX); } \
  NS_IMETHOD GetScrollY(PRInt32 *aScrollY) { return _to GetScrollY(aScrollY); } \
  NS_IMETHOD ScrollTo(PRInt32 x_scroll, PRInt32 y_scroll) { return _to ScrollTo(x_scroll, y_scroll); } \
  NS_IMETHOD ScrollBy(PRInt32 x_scroll_diff, PRInt32 y_scroll_diff) { return _to ScrollBy(x_scroll_diff, y_scroll_diff); } \
  NS_IMETHOD GetSelection(nsISelection **_retval) { return _to GetSelection(_retval); } \
  NS_IMETHOD ScrollByLines(PRInt32 num_lines) { return _to ScrollByLines(num_lines); } \
  NS_IMETHOD ScrollByPages(PRInt32 num_pages) { return _to ScrollByPages(num_pages); } \
  NS_IMETHOD SizeToContent(void) { return _to SizeToContent(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMWINDOW(_to) \
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDocument(aDocument); } \
  NS_IMETHOD GetParent(nsIDOMWindow * *aParent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParent(aParent); } \
  NS_IMETHOD GetTop(nsIDOMWindow * *aTop) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTop(aTop); } \
  NS_IMETHOD GetScrollbars(nsIDOMBarProp * *aScrollbars) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScrollbars(aScrollbars); } \
  NS_IMETHOD GetFrames(nsIDOMWindowCollection * *aFrames) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFrames(aFrames); } \
  NS_IMETHOD GetName(nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetName(aName); } \
  NS_IMETHOD SetName(const nsAString & aName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetName(aName); } \
  NS_IMETHOD GetTextZoom(float *aTextZoom) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTextZoom(aTextZoom); } \
  NS_IMETHOD SetTextZoom(float aTextZoom) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTextZoom(aTextZoom); } \
  NS_IMETHOD GetScrollX(PRInt32 *aScrollX) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScrollX(aScrollX); } \
  NS_IMETHOD GetScrollY(PRInt32 *aScrollY) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScrollY(aScrollY); } \
  NS_IMETHOD ScrollTo(PRInt32 x_scroll, PRInt32 y_scroll) { return !_to ? NS_ERROR_NULL_POINTER : _to->ScrollTo(x_scroll, y_scroll); } \
  NS_IMETHOD ScrollBy(PRInt32 x_scroll_diff, PRInt32 y_scroll_diff) { return !_to ? NS_ERROR_NULL_POINTER : _to->ScrollBy(x_scroll_diff, y_scroll_diff); } \
  NS_IMETHOD GetSelection(nsISelection **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSelection(_retval); } \
  NS_IMETHOD ScrollByLines(PRInt32 num_lines) { return !_to ? NS_ERROR_NULL_POINTER : _to->ScrollByLines(num_lines); } \
  NS_IMETHOD ScrollByPages(PRInt32 num_pages) { return !_to ? NS_ERROR_NULL_POINTER : _to->ScrollByPages(num_pages); } \
  NS_IMETHOD SizeToContent(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->SizeToContent(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMWindow : public nsIDOMWindow
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMWINDOW

  nsDOMWindow();

private:
  ~nsDOMWindow();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMWindow, nsIDOMWindow)

nsDOMWindow::nsDOMWindow()
{
  /* member initializers and constructor code */
}

nsDOMWindow::~nsDOMWindow()
{
  /* destructor code */
}

/* readonly attribute nsIDOMDocument document; */
NS_IMETHODIMP nsDOMWindow::GetDocument(nsIDOMDocument * *aDocument)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMWindow parent; */
NS_IMETHODIMP nsDOMWindow::GetParent(nsIDOMWindow * *aParent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMWindow top; */
NS_IMETHODIMP nsDOMWindow::GetTop(nsIDOMWindow * *aTop)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMBarProp scrollbars; */
NS_IMETHODIMP nsDOMWindow::GetScrollbars(nsIDOMBarProp * *aScrollbars)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] readonly attribute nsIDOMWindowCollection frames; */
NS_IMETHODIMP nsDOMWindow::GetFrames(nsIDOMWindowCollection * *aFrames)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute DOMString name; */
NS_IMETHODIMP nsDOMWindow::GetName(nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMWindow::SetName(const nsAString & aName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] attribute float textZoom; */
NS_IMETHODIMP nsDOMWindow::GetTextZoom(float *aTextZoom)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsDOMWindow::SetTextZoom(float aTextZoom)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long scrollX; */
NS_IMETHODIMP nsDOMWindow::GetScrollX(PRInt32 *aScrollX)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long scrollY; */
NS_IMETHODIMP nsDOMWindow::GetScrollY(PRInt32 *aScrollY)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void scrollTo (in long x_scroll, in long y_scroll); */
NS_IMETHODIMP nsDOMWindow::ScrollTo(PRInt32 x_scroll, PRInt32 y_scroll)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void scrollBy (in long x_scroll_diff, in long y_scroll_diff); */
NS_IMETHODIMP nsDOMWindow::ScrollBy(PRInt32 x_scroll_diff, PRInt32 y_scroll_diff)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsISelection getSelection (); */
NS_IMETHODIMP nsDOMWindow::GetSelection(nsISelection **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void scrollByLines (in long num_lines); */
NS_IMETHODIMP nsDOMWindow::ScrollByLines(PRInt32 num_lines)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void scrollByPages (in long num_pages); */
NS_IMETHODIMP nsDOMWindow::ScrollByPages(PRInt32 num_pages)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void sizeToContent (); */
NS_IMETHODIMP nsDOMWindow::SizeToContent()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMEventTarget; /* forward declaration */

class nsIDOMOfflineResourceList; /* forward declaration */


/* starting interface:    ns18IDOMWindow2 */
#define NS_18IDOMWINDOW2_IID_STR "65455132-b96a-40ec-adea-52fa22b1028c"

#define NS_18IDOMWINDOW2_IID \
  {0x65455132, 0xb96a, 0x40ec, \
    { 0xad, 0xea, 0x52, 0xfa, 0x22, 0xb1, 0x02, 0x8c }}

class NS_NO_VTABLE ns18IDOMWindow2 : public nsIDOMWindow {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_18IDOMWINDOW2_IID)

  /* [noscript] readonly attribute nsIDOMEventTarget windowRoot; */
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NS18IDOMWINDOW2 \
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NS18IDOMWINDOW2(_to) \
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot) { return _to GetWindowRoot(aWindowRoot); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NS18IDOMWINDOW2(_to) \
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWindowRoot(aWindowRoot); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public ns18IDOMWindow2
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NS18IDOMWINDOW2

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, ns18IDOMWindow2)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* [noscript] readonly attribute nsIDOMEventTarget windowRoot; */
NS_IMETHODIMP _MYCLASS_::GetWindowRoot(nsIDOMEventTarget * *aWindowRoot)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIDOMWindow2 */
#define NS_IDOMWINDOW2_IID_STR "73c5fa35-3add-4c87-a303-a850ccf4d65a"

#define NS_IDOMWINDOW2_IID \
  {0x73c5fa35, 0x3add, 0x4c87, \
    { 0xa3, 0x03, 0xa8, 0x50, 0xcc, 0xf4, 0xd6, 0x5a }}

class NS_NO_VTABLE nsIDOMWindow2 : public nsIDOMWindow {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IDOMWINDOW2_IID)

  /* [noscript] readonly attribute nsIDOMEventTarget windowRoot; */
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot) = 0;

  /* readonly attribute nsIDOMOfflineResourceList applicationCache; */
  NS_IMETHOD GetApplicationCache(nsIDOMOfflineResourceList * *aApplicationCache) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIDOMWINDOW2 \
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot); \
  NS_IMETHOD GetApplicationCache(nsIDOMOfflineResourceList * *aApplicationCache); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIDOMWINDOW2(_to) \
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot) { return _to GetWindowRoot(aWindowRoot); } \
  NS_IMETHOD GetApplicationCache(nsIDOMOfflineResourceList * *aApplicationCache) { return _to GetApplicationCache(aApplicationCache); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIDOMWINDOW2(_to) \
  NS_IMETHOD GetWindowRoot(nsIDOMEventTarget * *aWindowRoot) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWindowRoot(aWindowRoot); } \
  NS_IMETHOD GetApplicationCache(nsIDOMOfflineResourceList * *aApplicationCache) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetApplicationCache(aApplicationCache); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsDOMWindow2 : public nsIDOMWindow2
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIDOMWINDOW2

  nsDOMWindow2();

private:
  ~nsDOMWindow2();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsDOMWindow2, nsIDOMWindow2)

nsDOMWindow2::nsDOMWindow2()
{
  /* member initializers and constructor code */
}

nsDOMWindow2::~nsDOMWindow2()
{
  /* destructor code */
}

/* [noscript] readonly attribute nsIDOMEventTarget windowRoot; */
NS_IMETHODIMP nsDOMWindow2::GetWindowRoot(nsIDOMEventTarget * *aWindowRoot)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMOfflineResourceList applicationCache; */
NS_IMETHODIMP nsDOMWindow2::GetApplicationCache(nsIDOMOfflineResourceList * *aApplicationCache)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIEmbeddingSiteWindow */
#define NS_IEMBEDDINGSITEWINDOW_IID_STR "3e5432cd-9568-4bd1-8cbe-d50aba110743"

#define NS_IEMBEDDINGSITEWINDOW_IID \
  {0x3e5432cd, 0x9568, 0x4bd1, \
    { 0x8c, 0xbe, 0xd5, 0x0a, 0xba, 0x11, 0x07, 0x43 }}

class NS_NO_VTABLE nsIEmbeddingSiteWindow : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IEMBEDDINGSITEWINDOW_IID)

  enum { DIM_FLAGS_POSITION = 1U };

  enum { DIM_FLAGS_SIZE_INNER = 2U };

  enum { DIM_FLAGS_SIZE_OUTER = 4U };

  /* void setDimensions (in unsigned long flags, in long x, in long y, in long cx, in long cy); */
  NS_IMETHOD SetDimensions(PRUint32 flags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy) = 0;

  /* void getDimensions (in unsigned long flags, out long x, out long y, out long cx, out long cy); */
  NS_IMETHOD GetDimensions(PRUint32 flags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy) = 0;

  /* void setFocus (); */
  NS_IMETHOD SetFocus(void) = 0;

  /* attribute boolean visibility; */
  NS_IMETHOD GetVisibility(PRBool *aVisibility) = 0;
  NS_IMETHOD SetVisibility(PRBool aVisibility) = 0;

  /* attribute wstring title; */
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) = 0;
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) = 0;

  /* [noscript] readonly attribute voidPtr siteWindow; */
  NS_IMETHOD GetSiteWindow(void * *aSiteWindow) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIEMBEDDINGSITEWINDOW \
  NS_IMETHOD SetDimensions(PRUint32 flags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy); \
  NS_IMETHOD GetDimensions(PRUint32 flags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy); \
  NS_IMETHOD SetFocus(void); \
  NS_IMETHOD GetVisibility(PRBool *aVisibility); \
  NS_IMETHOD SetVisibility(PRBool aVisibility); \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle); \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle); \
  NS_IMETHOD GetSiteWindow(void * *aSiteWindow); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIEMBEDDINGSITEWINDOW(_to) \
  NS_IMETHOD SetDimensions(PRUint32 flags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy) { return _to SetDimensions(flags, x, y, cx, cy); } \
  NS_IMETHOD GetDimensions(PRUint32 flags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy) { return _to GetDimensions(flags, x, y, cx, cy); } \
  NS_IMETHOD SetFocus(void) { return _to SetFocus(); } \
  NS_IMETHOD GetVisibility(PRBool *aVisibility) { return _to GetVisibility(aVisibility); } \
  NS_IMETHOD SetVisibility(PRBool aVisibility) { return _to SetVisibility(aVisibility); } \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) { return _to GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) { return _to SetTitle(aTitle); } \
  NS_IMETHOD GetSiteWindow(void * *aSiteWindow) { return _to GetSiteWindow(aSiteWindow); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIEMBEDDINGSITEWINDOW(_to) \
  NS_IMETHOD SetDimensions(PRUint32 flags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDimensions(flags, x, y, cx, cy); } \
  NS_IMETHOD GetDimensions(PRUint32 flags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDimensions(flags, x, y, cx, cy); } \
  NS_IMETHOD SetFocus(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFocus(); } \
  NS_IMETHOD GetVisibility(PRBool *aVisibility) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetVisibility(aVisibility); } \
  NS_IMETHOD SetVisibility(PRBool aVisibility) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetVisibility(aVisibility); } \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTitle(aTitle); } \
  NS_IMETHOD GetSiteWindow(void * *aSiteWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSiteWindow(aSiteWindow); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsEmbeddingSiteWindow : public nsIEmbeddingSiteWindow
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIEMBEDDINGSITEWINDOW

  nsEmbeddingSiteWindow();

private:
  ~nsEmbeddingSiteWindow();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsEmbeddingSiteWindow, nsIEmbeddingSiteWindow)

nsEmbeddingSiteWindow::nsEmbeddingSiteWindow()
{
  /* member initializers and constructor code */
}

nsEmbeddingSiteWindow::~nsEmbeddingSiteWindow()
{
  /* destructor code */
}

/* void setDimensions (in unsigned long flags, in long x, in long y, in long cx, in long cy); */
NS_IMETHODIMP nsEmbeddingSiteWindow::SetDimensions(PRUint32 flags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getDimensions (in unsigned long flags, out long x, out long y, out long cx, out long cy); */
NS_IMETHODIMP nsEmbeddingSiteWindow::GetDimensions(PRUint32 flags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setFocus (); */
NS_IMETHODIMP nsEmbeddingSiteWindow::SetFocus()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean visibility; */
NS_IMETHODIMP nsEmbeddingSiteWindow::GetVisibility(PRBool *aVisibility)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsEmbeddingSiteWindow::SetVisibility(PRBool aVisibility)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring title; */
NS_IMETHODIMP nsEmbeddingSiteWindow::GetTitle(PRUnichar * *aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsEmbeddingSiteWindow::SetTitle(const PRUnichar * aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] readonly attribute voidPtr siteWindow; */
NS_IMETHODIMP nsEmbeddingSiteWindow::GetSiteWindow(void * *aSiteWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIEmbeddingSiteWindow2 */
#define NS_IEMBEDDINGSITEWINDOW2_IID_STR "e932bf55-0a64-4beb-923a-1f32d3661044"

#define NS_IEMBEDDINGSITEWINDOW2_IID \
  {0xe932bf55, 0x0a64, 0x4beb, \
    { 0x92, 0x3a, 0x1f, 0x32, 0xd3, 0x66, 0x10, 0x44 }}

class NS_NO_VTABLE nsIEmbeddingSiteWindow2 : public nsIEmbeddingSiteWindow {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IEMBEDDINGSITEWINDOW2_IID)

  /* void blur (); */
  NS_IMETHOD Blur(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIEMBEDDINGSITEWINDOW2 \
  NS_IMETHOD Blur(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIEMBEDDINGSITEWINDOW2(_to) \
  NS_IMETHOD Blur(void) { return _to Blur(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIEMBEDDINGSITEWINDOW2(_to) \
  NS_IMETHOD Blur(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Blur(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsEmbeddingSiteWindow2 : public nsIEmbeddingSiteWindow2
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIEMBEDDINGSITEWINDOW2

  nsEmbeddingSiteWindow2();

private:
  ~nsEmbeddingSiteWindow2();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsEmbeddingSiteWindow2, nsIEmbeddingSiteWindow2)

nsEmbeddingSiteWindow2::nsEmbeddingSiteWindow2()
{
  /* member initializers and constructor code */
}

nsEmbeddingSiteWindow2::~nsEmbeddingSiteWindow2()
{
  /* destructor code */
}

/* void blur (); */
NS_IMETHODIMP nsEmbeddingSiteWindow2::Blur()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIWebProgressListener2; /* forward declaration */

class nsIFile; /* forward declaration */

class nsIURI; /* forward declaration */

class nsIMIMEInfo; /* forward declaration */


/* starting interface:    nsIHelperAppLauncher */
#define NS_IHELPERAPPLAUNCHER_IID_STR "99a0882d-2ff9-4659-9952-9ac531ba5592"

#define NS_IHELPERAPPLAUNCHER_IID \
  {0x99a0882d, 0x2ff9, 0x4659, \
    { 0x99, 0x52, 0x9a, 0xc5, 0x31, 0xba, 0x55, 0x92 }}

class NS_NO_VTABLE nsIHelperAppLauncher : public nsICancelable {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IHELPERAPPLAUNCHER_IID)

  /* readonly attribute nsIMIMEInfo MIMEInfo; */
  NS_IMETHOD GetMIMEInfo(nsIMIMEInfo * *aMIMEInfo) = 0;

  /* readonly attribute nsIURI source; */
  NS_IMETHOD GetSource(nsIURI * *aSource) = 0;

  /* readonly attribute AString suggestedFileName; */
  NS_IMETHOD GetSuggestedFileName(nsAString & aSuggestedFileName) = 0;

  /* void saveToDisk (in nsIFile new_file_location, in boolean remember_this_preference); */
  NS_IMETHOD SaveToDisk(nsIFile *new_file_location, PRBool remember_this_preference) = 0;

  /* void launchWithApplication (in nsIFile application, in boolean remember_this_preference); */
  NS_IMETHOD LaunchWithApplication(nsIFile *application, PRBool remember_this_preference) = 0;

  /* void setWebProgressListener (in nsIWebProgressListener2 web_progress_listener); */
  NS_IMETHOD SetWebProgressListener(nsIWebProgressListener2 *web_progress_listener) = 0;

  /* void closeProgressWindow (); */
  NS_IMETHOD CloseProgressWindow(void) = 0;

  /* readonly attribute nsIFile targetFile; */
  NS_IMETHOD GetTargetFile(nsIFile * *aTargetFile) = 0;

  /* readonly attribute PRTime timeDownloadStarted; */
  NS_IMETHOD GetTimeDownloadStarted(PRTime *aTimeDownloadStarted) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIHELPERAPPLAUNCHER \
  NS_IMETHOD GetMIMEInfo(nsIMIMEInfo * *aMIMEInfo); \
  NS_IMETHOD GetSource(nsIURI * *aSource); \
  NS_IMETHOD GetSuggestedFileName(nsAString & aSuggestedFileName); \
  NS_IMETHOD SaveToDisk(nsIFile *new_file_location, PRBool remember_this_preference); \
  NS_IMETHOD LaunchWithApplication(nsIFile *application, PRBool remember_this_preference); \
  NS_IMETHOD SetWebProgressListener(nsIWebProgressListener2 *web_progress_listener); \
  NS_IMETHOD CloseProgressWindow(void); \
  NS_IMETHOD GetTargetFile(nsIFile * *aTargetFile); \
  NS_IMETHOD GetTimeDownloadStarted(PRTime *aTimeDownloadStarted); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIHELPERAPPLAUNCHER(_to) \
  NS_IMETHOD GetMIMEInfo(nsIMIMEInfo * *aMIMEInfo) { return _to GetMIMEInfo(aMIMEInfo); } \
  NS_IMETHOD GetSource(nsIURI * *aSource) { return _to GetSource(aSource); } \
  NS_IMETHOD GetSuggestedFileName(nsAString & aSuggestedFileName) { return _to GetSuggestedFileName(aSuggestedFileName); } \
  NS_IMETHOD SaveToDisk(nsIFile *new_file_location, PRBool remember_this_preference) { return _to SaveToDisk(new_file_location, remember_this_preference); } \
  NS_IMETHOD LaunchWithApplication(nsIFile *application, PRBool remember_this_preference) { return _to LaunchWithApplication(application, remember_this_preference); } \
  NS_IMETHOD SetWebProgressListener(nsIWebProgressListener2 *web_progress_listener) { return _to SetWebProgressListener(web_progress_listener); } \
  NS_IMETHOD CloseProgressWindow(void) { return _to CloseProgressWindow(); } \
  NS_IMETHOD GetTargetFile(nsIFile * *aTargetFile) { return _to GetTargetFile(aTargetFile); } \
  NS_IMETHOD GetTimeDownloadStarted(PRTime *aTimeDownloadStarted) { return _to GetTimeDownloadStarted(aTimeDownloadStarted); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIHELPERAPPLAUNCHER(_to) \
  NS_IMETHOD GetMIMEInfo(nsIMIMEInfo * *aMIMEInfo) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMIMEInfo(aMIMEInfo); } \
  NS_IMETHOD GetSource(nsIURI * *aSource) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSource(aSource); } \
  NS_IMETHOD GetSuggestedFileName(nsAString & aSuggestedFileName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSuggestedFileName(aSuggestedFileName); } \
  NS_IMETHOD SaveToDisk(nsIFile *new_file_location, PRBool remember_this_preference) { return !_to ? NS_ERROR_NULL_POINTER : _to->SaveToDisk(new_file_location, remember_this_preference); } \
  NS_IMETHOD LaunchWithApplication(nsIFile *application, PRBool remember_this_preference) { return !_to ? NS_ERROR_NULL_POINTER : _to->LaunchWithApplication(application, remember_this_preference); } \
  NS_IMETHOD SetWebProgressListener(nsIWebProgressListener2 *web_progress_listener) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetWebProgressListener(web_progress_listener); } \
  NS_IMETHOD CloseProgressWindow(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CloseProgressWindow(); } \
  NS_IMETHOD GetTargetFile(nsIFile * *aTargetFile) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTargetFile(aTargetFile); } \
  NS_IMETHOD GetTimeDownloadStarted(PRTime *aTimeDownloadStarted) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTimeDownloadStarted(aTimeDownloadStarted); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsHelperAppLauncher : public nsIHelperAppLauncher
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIHELPERAPPLAUNCHER

  nsHelperAppLauncher();

private:
  ~nsHelperAppLauncher();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsHelperAppLauncher, nsIHelperAppLauncher)

nsHelperAppLauncher::nsHelperAppLauncher()
{
  /* member initializers and constructor code */
}

nsHelperAppLauncher::~nsHelperAppLauncher()
{
  /* destructor code */
}

/* readonly attribute nsIMIMEInfo MIMEInfo; */
NS_IMETHODIMP nsHelperAppLauncher::GetMIMEInfo(nsIMIMEInfo * *aMIMEInfo)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIURI source; */
NS_IMETHODIMP nsHelperAppLauncher::GetSource(nsIURI * *aSource)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString suggestedFileName; */
NS_IMETHODIMP nsHelperAppLauncher::GetSuggestedFileName(nsAString & aSuggestedFileName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void saveToDisk (in nsIFile new_file_location, in boolean remember_this_preference); */
NS_IMETHODIMP nsHelperAppLauncher::SaveToDisk(nsIFile *new_file_location, PRBool remember_this_preference)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void launchWithApplication (in nsIFile application, in boolean remember_this_preference); */
NS_IMETHODIMP nsHelperAppLauncher::LaunchWithApplication(nsIFile *application, PRBool remember_this_preference)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setWebProgressListener (in nsIWebProgressListener2 web_progress_listener); */
NS_IMETHODIMP nsHelperAppLauncher::SetWebProgressListener(nsIWebProgressListener2 *web_progress_listener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void closeProgressWindow (); */
NS_IMETHODIMP nsHelperAppLauncher::CloseProgressWindow()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIFile targetFile; */
NS_IMETHODIMP nsHelperAppLauncher::GetTargetFile(nsIFile * *aTargetFile)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute PRTime timeDownloadStarted; */
NS_IMETHODIMP nsHelperAppLauncher::GetTimeDownloadStarted(PRTime *aTimeDownloadStarted)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIHelperAppLauncher; /* forward declaration */

class nsILocalFile; /* forward declaration */


/* starting interface:    nsIHelperAppLauncherDialog */
#define NS_IHELPERAPPLAUNCHERDIALOG_IID_STR "64355793-988d-40a5-ba8e-fcde78cac631"

#define NS_IHELPERAPPLAUNCHERDIALOG_IID \
  {0x64355793, 0x988d, 0x40a5, \
    { 0xba, 0x8e, 0xfc, 0xde, 0x78, 0xca, 0xc6, 0x31 }}

class NS_NO_VTABLE nsIHelperAppLauncherDialog : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IHELPERAPPLAUNCHERDIALOG_IID)

  enum { REASON_CANTHANDLE = 0U };

  enum { REASON_SERVERREQUEST = 1U };

  enum { REASON_TYPESNIFFED = 2U };

  /* void show (in nsIHelperAppLauncher launcher, in nsISupports windowContext, in unsigned long reason); */
  NS_IMETHOD Show(nsIHelperAppLauncher *launcher, nsISupports *windowContext, PRUint32 reason) = 0;

  /* nsILocalFile promptForSaveToFile (in nsIHelperAppLauncher launcher, in nsISupports windowContext, in wstring defaultFile, in wstring suggestedFileExtension); */
  NS_IMETHOD PromptForSaveToFile(nsIHelperAppLauncher *launcher, nsISupports *windowContext, const PRUnichar *defaultFile, const PRUnichar *suggestedFileExtension, nsILocalFile **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIHELPERAPPLAUNCHERDIALOG \
  NS_IMETHOD Show(nsIHelperAppLauncher *launcher, nsISupports *windowContext, PRUint32 reason); \
  NS_IMETHOD PromptForSaveToFile(nsIHelperAppLauncher *launcher, nsISupports *windowContext, const PRUnichar *defaultFile, const PRUnichar *suggestedFileExtension, nsILocalFile **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIHELPERAPPLAUNCHERDIALOG(_to) \
  NS_IMETHOD Show(nsIHelperAppLauncher *launcher, nsISupports *windowContext, PRUint32 reason) { return _to Show(launcher, windowContext, reason); } \
  NS_IMETHOD PromptForSaveToFile(nsIHelperAppLauncher *launcher, nsISupports *windowContext, const PRUnichar *defaultFile, const PRUnichar *suggestedFileExtension, nsILocalFile **_retval) { return _to PromptForSaveToFile(launcher, windowContext, defaultFile, suggestedFileExtension, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIHELPERAPPLAUNCHERDIALOG(_to) \
  NS_IMETHOD Show(nsIHelperAppLauncher *launcher, nsISupports *windowContext, PRUint32 reason) { return !_to ? NS_ERROR_NULL_POINTER : _to->Show(launcher, windowContext, reason); } \
  NS_IMETHOD PromptForSaveToFile(nsIHelperAppLauncher *launcher, nsISupports *windowContext, const PRUnichar *defaultFile, const PRUnichar *suggestedFileExtension, nsILocalFile **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->PromptForSaveToFile(launcher, windowContext, defaultFile, suggestedFileExtension, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsHelperAppLauncherDialog : public nsIHelperAppLauncherDialog
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIHELPERAPPLAUNCHERDIALOG

  nsHelperAppLauncherDialog();

private:
  ~nsHelperAppLauncherDialog();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsHelperAppLauncherDialog, nsIHelperAppLauncherDialog)

nsHelperAppLauncherDialog::nsHelperAppLauncherDialog()
{
  /* member initializers and constructor code */
}

nsHelperAppLauncherDialog::~nsHelperAppLauncherDialog()
{
  /* destructor code */
}

/* void show (in nsIHelperAppLauncher launcher, in nsISupports windowContext, in unsigned long reason); */
NS_IMETHODIMP nsHelperAppLauncherDialog::Show(nsIHelperAppLauncher *launcher, nsISupports *windowContext, PRUint32 reason)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsILocalFile promptForSaveToFile (in nsIHelperAppLauncher launcher, in nsISupports windowContext, in wstring defaultFile, in wstring suggestedFileExtension); */
NS_IMETHODIMP nsHelperAppLauncherDialog::PromptForSaveToFile(nsIHelperAppLauncher *launcher, nsISupports *windowContext, const PRUnichar *defaultFile, const PRUnichar *suggestedFileExtension, nsILocalFile **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

typedef NS_CALLBACK(nsWriteSegmentFun)(
        nsIInputStream* in_stream,
        void* closure,
        const char* from_segment,
        PRUint32 to_offset,
        PRUint32 count,
        PRUint32* write_count
);

/* starting interface:    nsIInputStream */
#define NS_IINPUTSTREAM_IID_STR "fa9c7f6c-61b3-11d4-9877-00c04fa0cf4a"

#define NS_IINPUTSTREAM_IID \
  {0xfa9c7f6c, 0x61b3, 0x11d4, \
    { 0x98, 0x77, 0x00, 0xc0, 0x4f, 0xa0, 0xcf, 0x4a }}

class NS_NO_VTABLE nsIInputStream : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IINPUTSTREAM_IID)

  /* void close (); */
  NS_IMETHOD Close(void) = 0;

  /* unsigned long available (); */
  NS_IMETHOD Available(PRUint32 *_retval) = 0;

  /* [noscript] unsigned long read (in charPtr buf, in unsigned long count); */
  NS_IMETHOD Read(char * buf, PRUint32 count, PRUint32 *_retval) = 0;

  /* [noscript] unsigned long readSegments (in voidPtr writer, in voidPtr closure, in unsigned long count); */
  NS_IMETHOD ReadSegments(void * writer, void * closure, PRUint32 count, PRUint32 *_retval) = 0;

  /* boolean isNonBlocking (); */
  NS_IMETHOD IsNonBlocking(PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIINPUTSTREAM \
  NS_IMETHOD Close(void); \
  NS_IMETHOD Available(PRUint32 *_retval); \
  NS_IMETHOD Read(char * buf, PRUint32 count, PRUint32 *_retval); \
  NS_IMETHOD ReadSegments(void * writer, void * closure, PRUint32 count, PRUint32 *_retval); \
  NS_IMETHOD IsNonBlocking(PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIINPUTSTREAM(_to) \
  NS_IMETHOD Close(void) { return _to Close(); } \
  NS_IMETHOD Available(PRUint32 *_retval) { return _to Available(_retval); } \
  NS_IMETHOD Read(char * buf, PRUint32 count, PRUint32 *_retval) { return _to Read(buf, count, _retval); } \
  NS_IMETHOD ReadSegments(void * writer, void * closure, PRUint32 count, PRUint32 *_retval) { return _to ReadSegments(writer, closure, count, _retval); } \
  NS_IMETHOD IsNonBlocking(PRBool *_retval) { return _to IsNonBlocking(_retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIINPUTSTREAM(_to) \
  NS_IMETHOD Close(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Close(); } \
  NS_IMETHOD Available(PRUint32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Available(_retval); } \
  NS_IMETHOD Read(char * buf, PRUint32 count, PRUint32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Read(buf, count, _retval); } \
  NS_IMETHOD ReadSegments(void * writer, void * closure, PRUint32 count, PRUint32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadSegments(writer, closure, count, _retval); } \
  NS_IMETHOD IsNonBlocking(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsNonBlocking(_retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsInputStream : public nsIInputStream
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIINPUTSTREAM

  nsInputStream();

private:
  ~nsInputStream();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsInputStream, nsIInputStream)

nsInputStream::nsInputStream()
{
  /* member initializers and constructor code */
}

nsInputStream::~nsInputStream()
{
  /* destructor code */
}

/* void close (); */
NS_IMETHODIMP nsInputStream::Close()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* unsigned long available (); */
NS_IMETHODIMP nsInputStream::Available(PRUint32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] unsigned long read (in charPtr buf, in unsigned long count); */
NS_IMETHODIMP nsInputStream::Read(char * buf, PRUint32 count, PRUint32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] unsigned long readSegments (in voidPtr writer, in voidPtr closure, in unsigned long count); */
NS_IMETHODIMP nsInputStream::ReadSegments(void * writer, void * closure, PRUint32 count, PRUint32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isNonBlocking (); */
NS_IMETHODIMP nsInputStream::IsNonBlocking(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIProtocolHandler; /* forward declaration */

class nsIURI; /* forward declaration */

class nsIFile; /* forward declaration */

class nsIChannel; /* forward declaration */


/* starting interface:    nsIIOService */
#define NS_IIOSERVICE_IID_STR "bddeda3f-9020-4d12-8c70-984ee9f7935e"

#define NS_IIOSERVICE_IID \
  {0xbddeda3f, 0x9020, 0x4d12, \
    { 0x8c, 0x70, 0x98, 0x4e, 0xe9, 0xf7, 0x93, 0x5e }}

class NS_NO_VTABLE nsIIOService : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IIOSERVICE_IID)

  /* nsIProtocolHandler getProtocolHandler (in string scheme); */
  NS_IMETHOD GetProtocolHandler(const char *scheme, nsIProtocolHandler **_retval) = 0;

  /* unsigned long getProtocolFlags (in string scheme); */
  NS_IMETHOD GetProtocolFlags(const char *scheme, PRUint32 *_retval) = 0;

  /* nsIURI newURI (in AUTF8String spec, in string origin_charset, in nsIURI base_uri); */
  NS_IMETHOD NewURI(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIURI **_retval) = 0;

  /* nsIURI newFileURI (in nsIFile file); */
  NS_IMETHOD NewFileURI(nsIFile *file, nsIURI **_retval) = 0;

  /* nsIChannel newChannelFromURI (in nsIURI uri); */
  NS_IMETHOD NewChannelFromURI(nsIURI *uri, nsIChannel **_retval) = 0;

  /* nsIChannel newChannel (in AUTF8String spec, in string origin_charset, in nsIURI base_uri); */
  NS_IMETHOD NewChannel(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIChannel **_retval) = 0;

  /* attribute boolean offline; */
  NS_IMETHOD GetOffline(PRBool *aOffline) = 0;
  NS_IMETHOD SetOffline(PRBool aOffline) = 0;

  /* boolean allowPort (in long port, in string scheme); */
  NS_IMETHOD AllowPort(PRInt32 port, const char *scheme, PRBool *_retval) = 0;

  /* ACString extractScheme (in AUTF8String url_string); */
  NS_IMETHOD ExtractScheme(const nsACString & url_string, nsACString & _retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIIOSERVICE \
  NS_IMETHOD GetProtocolHandler(const char *scheme, nsIProtocolHandler **_retval); \
  NS_IMETHOD GetProtocolFlags(const char *scheme, PRUint32 *_retval); \
  NS_IMETHOD NewURI(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIURI **_retval); \
  NS_IMETHOD NewFileURI(nsIFile *file, nsIURI **_retval); \
  NS_IMETHOD NewChannelFromURI(nsIURI *uri, nsIChannel **_retval); \
  NS_IMETHOD NewChannel(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIChannel **_retval); \
  NS_IMETHOD GetOffline(PRBool *aOffline); \
  NS_IMETHOD SetOffline(PRBool aOffline); \
  NS_IMETHOD AllowPort(PRInt32 port, const char *scheme, PRBool *_retval); \
  NS_IMETHOD ExtractScheme(const nsACString & url_string, nsACString & _retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIIOSERVICE(_to) \
  NS_IMETHOD GetProtocolHandler(const char *scheme, nsIProtocolHandler **_retval) { return _to GetProtocolHandler(scheme, _retval); } \
  NS_IMETHOD GetProtocolFlags(const char *scheme, PRUint32 *_retval) { return _to GetProtocolFlags(scheme, _retval); } \
  NS_IMETHOD NewURI(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIURI **_retval) { return _to NewURI(spec, origin_charset, base_uri, _retval); } \
  NS_IMETHOD NewFileURI(nsIFile *file, nsIURI **_retval) { return _to NewFileURI(file, _retval); } \
  NS_IMETHOD NewChannelFromURI(nsIURI *uri, nsIChannel **_retval) { return _to NewChannelFromURI(uri, _retval); } \
  NS_IMETHOD NewChannel(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIChannel **_retval) { return _to NewChannel(spec, origin_charset, base_uri, _retval); } \
  NS_IMETHOD GetOffline(PRBool *aOffline) { return _to GetOffline(aOffline); } \
  NS_IMETHOD SetOffline(PRBool aOffline) { return _to SetOffline(aOffline); } \
  NS_IMETHOD AllowPort(PRInt32 port, const char *scheme, PRBool *_retval) { return _to AllowPort(port, scheme, _retval); } \
  NS_IMETHOD ExtractScheme(const nsACString & url_string, nsACString & _retval) { return _to ExtractScheme(url_string, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIIOSERVICE(_to) \
  NS_IMETHOD GetProtocolHandler(const char *scheme, nsIProtocolHandler **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetProtocolHandler(scheme, _retval); } \
  NS_IMETHOD GetProtocolFlags(const char *scheme, PRUint32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetProtocolFlags(scheme, _retval); } \
  NS_IMETHOD NewURI(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIURI **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->NewURI(spec, origin_charset, base_uri, _retval); } \
  NS_IMETHOD NewFileURI(nsIFile *file, nsIURI **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->NewFileURI(file, _retval); } \
  NS_IMETHOD NewChannelFromURI(nsIURI *uri, nsIChannel **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->NewChannelFromURI(uri, _retval); } \
  NS_IMETHOD NewChannel(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIChannel **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->NewChannel(spec, origin_charset, base_uri, _retval); } \
  NS_IMETHOD GetOffline(PRBool *aOffline) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOffline(aOffline); } \
  NS_IMETHOD SetOffline(PRBool aOffline) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetOffline(aOffline); } \
  NS_IMETHOD AllowPort(PRInt32 port, const char *scheme, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->AllowPort(port, scheme, _retval); } \
  NS_IMETHOD ExtractScheme(const nsACString & url_string, nsACString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExtractScheme(url_string, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsIOService : public nsIIOService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIIOSERVICE

  nsIOService();

private:
  ~nsIOService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsIOService, nsIIOService)

nsIOService::nsIOService()
{
  /* member initializers and constructor code */
}

nsIOService::~nsIOService()
{
  /* destructor code */
}

/* nsIProtocolHandler getProtocolHandler (in string scheme); */
NS_IMETHODIMP nsIOService::GetProtocolHandler(const char *scheme, nsIProtocolHandler **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* unsigned long getProtocolFlags (in string scheme); */
NS_IMETHODIMP nsIOService::GetProtocolFlags(const char *scheme, PRUint32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIURI newURI (in AUTF8String spec, in string origin_charset, in nsIURI base_uri); */
NS_IMETHODIMP nsIOService::NewURI(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIURI **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIURI newFileURI (in nsIFile file); */
NS_IMETHODIMP nsIOService::NewFileURI(nsIFile *file, nsIURI **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIChannel newChannelFromURI (in nsIURI uri); */
NS_IMETHODIMP nsIOService::NewChannelFromURI(nsIURI *uri, nsIChannel **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIChannel newChannel (in AUTF8String spec, in string origin_charset, in nsIURI base_uri); */
NS_IMETHODIMP nsIOService::NewChannel(const nsACString & spec, const char *origin_charset, nsIURI *base_uri, nsIChannel **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean offline; */
NS_IMETHODIMP nsIOService::GetOffline(PRBool *aOffline)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsIOService::SetOffline(PRBool aOffline)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean allowPort (in long port, in string scheme); */
NS_IMETHODIMP nsIOService::AllowPort(PRInt32 port, const char *scheme, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* ACString extractScheme (in AUTF8String url_string); */
NS_IMETHODIMP nsIOService::ExtractScheme(const nsACString & url_string, nsACString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

struct JSContext;

/* starting interface:    nsIJSContextStack */
#define NS_IJSCONTEXTSTACK_IID_STR "c67d8270-3189-11d3-9885-006008962422"

#define NS_IJSCONTEXTSTACK_IID \
  {0xc67d8270, 0x3189, 0x11d3, \
    { 0x98, 0x85, 0x00, 0x60, 0x08, 0x96, 0x24, 0x22 }}

class NS_NO_VTABLE nsIJSContextStack : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IJSCONTEXTSTACK_IID)

  /* readonly attribute PRInt32 count; */
  NS_IMETHOD GetCount(PRInt32 *aCount) = 0;

  /* JSContext peek (); */
  NS_IMETHOD Peek(JSContext * *_retval) = 0;

  /* JSContext pop (); */
  NS_IMETHOD Pop(JSContext * *_retval) = 0;

  /* void push (in JSContext contxt); */
  NS_IMETHOD Push(JSContext * contxt) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIJSCONTEXTSTACK \
  NS_IMETHOD GetCount(PRInt32 *aCount); \
  NS_IMETHOD Peek(JSContext * *_retval); \
  NS_IMETHOD Pop(JSContext * *_retval); \
  NS_IMETHOD Push(JSContext * contxt); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIJSCONTEXTSTACK(_to) \
  NS_IMETHOD GetCount(PRInt32 *aCount) { return _to GetCount(aCount); } \
  NS_IMETHOD Peek(JSContext * *_retval) { return _to Peek(_retval); } \
  NS_IMETHOD Pop(JSContext * *_retval) { return _to Pop(_retval); } \
  NS_IMETHOD Push(JSContext * contxt) { return _to Push(contxt); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIJSCONTEXTSTACK(_to) \
  NS_IMETHOD GetCount(PRInt32 *aCount) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCount(aCount); } \
  NS_IMETHOD Peek(JSContext * *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Peek(_retval); } \
  NS_IMETHOD Pop(JSContext * *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Pop(_retval); } \
  NS_IMETHOD Push(JSContext * contxt) { return !_to ? NS_ERROR_NULL_POINTER : _to->Push(contxt); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsJSContextStack : public nsIJSContextStack
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIJSCONTEXTSTACK

  nsJSContextStack();

private:
  ~nsJSContextStack();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsJSContextStack, nsIJSContextStack)

nsJSContextStack::nsJSContextStack()
{
  /* member initializers and constructor code */
}

nsJSContextStack::~nsJSContextStack()
{
  /* destructor code */
}

/* readonly attribute PRInt32 count; */
NS_IMETHODIMP nsJSContextStack::GetCount(PRInt32 *aCount)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* JSContext peek (); */
NS_IMETHODIMP nsJSContextStack::Peek(JSContext * *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* JSContext pop (); */
NS_IMETHODIMP nsJSContextStack::Pop(JSContext * *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void push (in JSContext contxt); */
NS_IMETHODIMP nsJSContextStack::Push(JSContext * contxt)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIUTF8StringEnumerator; /* forward declaration */

class nsIFile; /* forward declaration */

class nsMIMEInfoHandleAction; /* forward declaration */


/* starting interface:    nsIMIMEInfo */
#define NS_IMIMEINFO_IID_STR "cd7083f8-5fe9-4248-bb09-0b0e2982fde8"

#define NS_IMIMEINFO_IID \
  {0xcd7083f8, 0x5fe9, 0x4248, \
    { 0xbb, 0x09, 0x0b, 0x0e, 0x29, 0x82, 0xfd, 0xe8 }}

class NS_NO_VTABLE nsIMIMEInfo : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IMIMEINFO_IID)

  /* nsIUTF8StringEnumerator getFileExtensions (); */
  NS_IMETHOD GetFileExtensions(nsIUTF8StringEnumerator **_retval) = 0;

  /* void setFileExtensions (in AUTF8String extensions); */
  NS_IMETHOD SetFileExtensions(const nsACString & extensions) = 0;

  /* boolean extensionExists (in AUTF8String extension); */
  NS_IMETHOD ExtensionExists(const nsACString & extension, PRBool *_retval) = 0;

  /* void appendExtension (in AUTF8String extension); */
  NS_IMETHOD AppendExtension(const nsACString & extension) = 0;

  /* attribute AUTF8String primaryExtension; */
  NS_IMETHOD GetPrimaryExtension(nsACString & aPrimaryExtension) = 0;
  NS_IMETHOD SetPrimaryExtension(const nsACString & aPrimaryExtension) = 0;

  /* readonly attribute ACString MIMEType; */
  NS_IMETHOD GetMIMEType(nsACString & aMIMEType) = 0;

  /* attribute AString description; */
  NS_IMETHOD GetDescription(nsAString & aDescription) = 0;
  NS_IMETHOD SetDescription(const nsAString & aDescription) = 0;

  /* attribute PRUint32 macType; */
  NS_IMETHOD GetMacType(PRUint32 *aMacType) = 0;
  NS_IMETHOD SetMacType(PRUint32 aMacType) = 0;

  /* attribute PRUint32 macCreator; */
  NS_IMETHOD GetMacCreator(PRUint32 *aMacCreator) = 0;
  NS_IMETHOD SetMacCreator(PRUint32 aMacCreator) = 0;

  /* boolean equals (in nsIMIMEInfo mime_info); */
  NS_IMETHOD Equals(nsIMIMEInfo *mime_info, PRBool *_retval) = 0;

  /* attribute nsIFile preferredApplicationHandler; */
  NS_IMETHOD GetPreferredApplicationHandler(nsIFile * *aPreferredApplicationHandler) = 0;
  NS_IMETHOD SetPreferredApplicationHandler(nsIFile * aPreferredApplicationHandler) = 0;

  /* attribute AString applicationDescription; */
  NS_IMETHOD GetApplicationDescription(nsAString & aApplicationDescription) = 0;
  NS_IMETHOD SetApplicationDescription(const nsAString & aApplicationDescription) = 0;

  /* readonly attribute boolean hasDefaultHandler; */
  NS_IMETHOD GetHasDefaultHandler(PRBool *aHasDefaultHandler) = 0;

  /* readonly attribute AString defaultDescription; */
  NS_IMETHOD GetDefaultDescription(nsAString & aDefaultDescription) = 0;

  /* void launchWithFile (in nsIFile file); */
  NS_IMETHOD LaunchWithFile(nsIFile *file) = 0;

  enum { saveToDisk = 0 };

  enum { alwaysAsk = 1 };

  enum { useHelperApp = 2 };

  enum { handleInternally = 3 };

  enum { useSystemDefault = 4 };

  /* attribute nsMIMEInfoHandleAction preferredAction; */
  NS_IMETHOD GetPreferredAction(nsMIMEInfoHandleAction * *aPreferredAction) = 0;
  NS_IMETHOD SetPreferredAction(nsMIMEInfoHandleAction * aPreferredAction) = 0;

  /* attribute boolean alwaysAskBeforeHandling; */
  NS_IMETHOD GetAlwaysAskBeforeHandling(PRBool *aAlwaysAskBeforeHandling) = 0;
  NS_IMETHOD SetAlwaysAskBeforeHandling(PRBool aAlwaysAskBeforeHandling) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIMIMEINFO \
  NS_IMETHOD GetFileExtensions(nsIUTF8StringEnumerator **_retval); \
  NS_IMETHOD SetFileExtensions(const nsACString & extensions); \
  NS_IMETHOD ExtensionExists(const nsACString & extension, PRBool *_retval); \
  NS_IMETHOD AppendExtension(const nsACString & extension); \
  NS_IMETHOD GetPrimaryExtension(nsACString & aPrimaryExtension); \
  NS_IMETHOD SetPrimaryExtension(const nsACString & aPrimaryExtension); \
  NS_IMETHOD GetMIMEType(nsACString & aMIMEType); \
  NS_IMETHOD GetDescription(nsAString & aDescription); \
  NS_IMETHOD SetDescription(const nsAString & aDescription); \
  NS_IMETHOD GetMacType(PRUint32 *aMacType); \
  NS_IMETHOD SetMacType(PRUint32 aMacType); \
  NS_IMETHOD GetMacCreator(PRUint32 *aMacCreator); \
  NS_IMETHOD SetMacCreator(PRUint32 aMacCreator); \
  NS_IMETHOD Equals(nsIMIMEInfo *mime_info, PRBool *_retval); \
  NS_IMETHOD GetPreferredApplicationHandler(nsIFile * *aPreferredApplicationHandler); \
  NS_IMETHOD SetPreferredApplicationHandler(nsIFile * aPreferredApplicationHandler); \
  NS_IMETHOD GetApplicationDescription(nsAString & aApplicationDescription); \
  NS_IMETHOD SetApplicationDescription(const nsAString & aApplicationDescription); \
  NS_IMETHOD GetHasDefaultHandler(PRBool *aHasDefaultHandler); \
  NS_IMETHOD GetDefaultDescription(nsAString & aDefaultDescription); \
  NS_IMETHOD LaunchWithFile(nsIFile *file); \
  NS_IMETHOD GetPreferredAction(nsMIMEInfoHandleAction * *aPreferredAction); \
  NS_IMETHOD SetPreferredAction(nsMIMEInfoHandleAction * aPreferredAction); \
  NS_IMETHOD GetAlwaysAskBeforeHandling(PRBool *aAlwaysAskBeforeHandling); \
  NS_IMETHOD SetAlwaysAskBeforeHandling(PRBool aAlwaysAskBeforeHandling); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIMIMEINFO(_to) \
  NS_IMETHOD GetFileExtensions(nsIUTF8StringEnumerator **_retval) { return _to GetFileExtensions(_retval); } \
  NS_IMETHOD SetFileExtensions(const nsACString & extensions) { return _to SetFileExtensions(extensions); } \
  NS_IMETHOD ExtensionExists(const nsACString & extension, PRBool *_retval) { return _to ExtensionExists(extension, _retval); } \
  NS_IMETHOD AppendExtension(const nsACString & extension) { return _to AppendExtension(extension); } \
  NS_IMETHOD GetPrimaryExtension(nsACString & aPrimaryExtension) { return _to GetPrimaryExtension(aPrimaryExtension); } \
  NS_IMETHOD SetPrimaryExtension(const nsACString & aPrimaryExtension) { return _to SetPrimaryExtension(aPrimaryExtension); } \
  NS_IMETHOD GetMIMEType(nsACString & aMIMEType) { return _to GetMIMEType(aMIMEType); } \
  NS_IMETHOD GetDescription(nsAString & aDescription) { return _to GetDescription(aDescription); } \
  NS_IMETHOD SetDescription(const nsAString & aDescription) { return _to SetDescription(aDescription); } \
  NS_IMETHOD GetMacType(PRUint32 *aMacType) { return _to GetMacType(aMacType); } \
  NS_IMETHOD SetMacType(PRUint32 aMacType) { return _to SetMacType(aMacType); } \
  NS_IMETHOD GetMacCreator(PRUint32 *aMacCreator) { return _to GetMacCreator(aMacCreator); } \
  NS_IMETHOD SetMacCreator(PRUint32 aMacCreator) { return _to SetMacCreator(aMacCreator); } \
  NS_IMETHOD Equals(nsIMIMEInfo *mime_info, PRBool *_retval) { return _to Equals(mime_info, _retval); } \
  NS_IMETHOD GetPreferredApplicationHandler(nsIFile * *aPreferredApplicationHandler) { return _to GetPreferredApplicationHandler(aPreferredApplicationHandler); } \
  NS_IMETHOD SetPreferredApplicationHandler(nsIFile * aPreferredApplicationHandler) { return _to SetPreferredApplicationHandler(aPreferredApplicationHandler); } \
  NS_IMETHOD GetApplicationDescription(nsAString & aApplicationDescription) { return _to GetApplicationDescription(aApplicationDescription); } \
  NS_IMETHOD SetApplicationDescription(const nsAString & aApplicationDescription) { return _to SetApplicationDescription(aApplicationDescription); } \
  NS_IMETHOD GetHasDefaultHandler(PRBool *aHasDefaultHandler) { return _to GetHasDefaultHandler(aHasDefaultHandler); } \
  NS_IMETHOD GetDefaultDescription(nsAString & aDefaultDescription) { return _to GetDefaultDescription(aDefaultDescription); } \
  NS_IMETHOD LaunchWithFile(nsIFile *file) { return _to LaunchWithFile(file); } \
  NS_IMETHOD GetPreferredAction(nsMIMEInfoHandleAction * *aPreferredAction) { return _to GetPreferredAction(aPreferredAction); } \
  NS_IMETHOD SetPreferredAction(nsMIMEInfoHandleAction * aPreferredAction) { return _to SetPreferredAction(aPreferredAction); } \
  NS_IMETHOD GetAlwaysAskBeforeHandling(PRBool *aAlwaysAskBeforeHandling) { return _to GetAlwaysAskBeforeHandling(aAlwaysAskBeforeHandling); } \
  NS_IMETHOD SetAlwaysAskBeforeHandling(PRBool aAlwaysAskBeforeHandling) { return _to SetAlwaysAskBeforeHandling(aAlwaysAskBeforeHandling); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIMIMEINFO(_to) \
  NS_IMETHOD GetFileExtensions(nsIUTF8StringEnumerator **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFileExtensions(_retval); } \
  NS_IMETHOD SetFileExtensions(const nsACString & extensions) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFileExtensions(extensions); } \
  NS_IMETHOD ExtensionExists(const nsACString & extension, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExtensionExists(extension, _retval); } \
  NS_IMETHOD AppendExtension(const nsACString & extension) { return !_to ? NS_ERROR_NULL_POINTER : _to->AppendExtension(extension); } \
  NS_IMETHOD GetPrimaryExtension(nsACString & aPrimaryExtension) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrimaryExtension(aPrimaryExtension); } \
  NS_IMETHOD SetPrimaryExtension(const nsACString & aPrimaryExtension) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrimaryExtension(aPrimaryExtension); } \
  NS_IMETHOD GetMIMEType(nsACString & aMIMEType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMIMEType(aMIMEType); } \
  NS_IMETHOD GetDescription(nsAString & aDescription) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDescription(aDescription); } \
  NS_IMETHOD SetDescription(const nsAString & aDescription) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDescription(aDescription); } \
  NS_IMETHOD GetMacType(PRUint32 *aMacType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMacType(aMacType); } \
  NS_IMETHOD SetMacType(PRUint32 aMacType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMacType(aMacType); } \
  NS_IMETHOD GetMacCreator(PRUint32 *aMacCreator) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMacCreator(aMacCreator); } \
  NS_IMETHOD SetMacCreator(PRUint32 aMacCreator) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMacCreator(aMacCreator); } \
  NS_IMETHOD Equals(nsIMIMEInfo *mime_info, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Equals(mime_info, _retval); } \
  NS_IMETHOD GetPreferredApplicationHandler(nsIFile * *aPreferredApplicationHandler) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPreferredApplicationHandler(aPreferredApplicationHandler); } \
  NS_IMETHOD SetPreferredApplicationHandler(nsIFile * aPreferredApplicationHandler) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPreferredApplicationHandler(aPreferredApplicationHandler); } \
  NS_IMETHOD GetApplicationDescription(nsAString & aApplicationDescription) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetApplicationDescription(aApplicationDescription); } \
  NS_IMETHOD SetApplicationDescription(const nsAString & aApplicationDescription) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetApplicationDescription(aApplicationDescription); } \
  NS_IMETHOD GetHasDefaultHandler(PRBool *aHasDefaultHandler) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHasDefaultHandler(aHasDefaultHandler); } \
  NS_IMETHOD GetDefaultDescription(nsAString & aDefaultDescription) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultDescription(aDefaultDescription); } \
  NS_IMETHOD LaunchWithFile(nsIFile *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->LaunchWithFile(file); } \
  NS_IMETHOD GetPreferredAction(nsMIMEInfoHandleAction * *aPreferredAction) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPreferredAction(aPreferredAction); } \
  NS_IMETHOD SetPreferredAction(nsMIMEInfoHandleAction * aPreferredAction) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPreferredAction(aPreferredAction); } \
  NS_IMETHOD GetAlwaysAskBeforeHandling(PRBool *aAlwaysAskBeforeHandling) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAlwaysAskBeforeHandling(aAlwaysAskBeforeHandling); } \
  NS_IMETHOD SetAlwaysAskBeforeHandling(PRBool aAlwaysAskBeforeHandling) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAlwaysAskBeforeHandling(aAlwaysAskBeforeHandling); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsMIMEInfo : public nsIMIMEInfo
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIMIMEINFO

  nsMIMEInfo();

private:
  ~nsMIMEInfo();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsMIMEInfo, nsIMIMEInfo)

nsMIMEInfo::nsMIMEInfo()
{
  /* member initializers and constructor code */
}

nsMIMEInfo::~nsMIMEInfo()
{
  /* destructor code */
}

/* nsIUTF8StringEnumerator getFileExtensions (); */
NS_IMETHODIMP nsMIMEInfo::GetFileExtensions(nsIUTF8StringEnumerator **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setFileExtensions (in AUTF8String extensions); */
NS_IMETHODIMP nsMIMEInfo::SetFileExtensions(const nsACString & extensions)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean extensionExists (in AUTF8String extension); */
NS_IMETHODIMP nsMIMEInfo::ExtensionExists(const nsACString & extension, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void appendExtension (in AUTF8String extension); */
NS_IMETHODIMP nsMIMEInfo::AppendExtension(const nsACString & extension)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AUTF8String primaryExtension; */
NS_IMETHODIMP nsMIMEInfo::GetPrimaryExtension(nsACString & aPrimaryExtension)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetPrimaryExtension(const nsACString & aPrimaryExtension)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute ACString MIMEType; */
NS_IMETHODIMP nsMIMEInfo::GetMIMEType(nsACString & aMIMEType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AString description; */
NS_IMETHODIMP nsMIMEInfo::GetDescription(nsAString & aDescription)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetDescription(const nsAString & aDescription)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute PRUint32 macType; */
NS_IMETHODIMP nsMIMEInfo::GetMacType(PRUint32 *aMacType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetMacType(PRUint32 aMacType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute PRUint32 macCreator; */
NS_IMETHODIMP nsMIMEInfo::GetMacCreator(PRUint32 *aMacCreator)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetMacCreator(PRUint32 aMacCreator)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean equals (in nsIMIMEInfo mime_info); */
NS_IMETHODIMP nsMIMEInfo::Equals(nsIMIMEInfo *mime_info, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIFile preferredApplicationHandler; */
NS_IMETHODIMP nsMIMEInfo::GetPreferredApplicationHandler(nsIFile * *aPreferredApplicationHandler)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetPreferredApplicationHandler(nsIFile * aPreferredApplicationHandler)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AString applicationDescription; */
NS_IMETHODIMP nsMIMEInfo::GetApplicationDescription(nsAString & aApplicationDescription)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetApplicationDescription(const nsAString & aApplicationDescription)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean hasDefaultHandler; */
NS_IMETHODIMP nsMIMEInfo::GetHasDefaultHandler(PRBool *aHasDefaultHandler)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString defaultDescription; */
NS_IMETHODIMP nsMIMEInfo::GetDefaultDescription(nsAString & aDefaultDescription)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void launchWithFile (in nsIFile file); */
NS_IMETHODIMP nsMIMEInfo::LaunchWithFile(nsIFile *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsMIMEInfoHandleAction preferredAction; */
NS_IMETHODIMP nsMIMEInfo::GetPreferredAction(nsMIMEInfoHandleAction * *aPreferredAction)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetPreferredAction(nsMIMEInfoHandleAction * aPreferredAction)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean alwaysAskBeforeHandling; */
NS_IMETHODIMP nsMIMEInfo::GetAlwaysAskBeforeHandling(PRBool *aAlwaysAskBeforeHandling)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsMIMEInfo::SetAlwaysAskBeforeHandling(PRBool aAlwaysAskBeforeHandling)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIFileSpec; /* forward declaration */

class nsIFile; /* forward declaration */

class nsILocalFile; /* forward declaration */

class nsIPrefBranch; /* forward declaration */

class nsIObserver; /* forward declaration */

    typedef int (*PrefChangedFunc)(const char*, void*);
    typedef void (*PrefEnumerationFunc)(const char *, void *);

/* starting interface:    nsIPref */
#define NS_IPREF_IID_STR "a22ad7b0-ca86-11d1-a9a4-00805f8a7ac4"

#define NS_IPREF_IID \
  {0xa22ad7b0, 0xca86, 0x11d1, \
    { 0xa9, 0xa4, 0x00, 0x80, 0x5f, 0x8a, 0x7a, 0xc4 }}

class NS_NO_VTABLE nsIPref : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IPREF_IID)

  /* void readUserPrefs (in nsIFile file); */
  NS_IMETHOD ReadUserPrefs(nsIFile *file) = 0;

  /* void ResetPrefs (); */
  NS_IMETHOD ResetPrefs(void) = 0;

  /* void ResetUserPrefs (); */
  NS_IMETHOD ResetUserPrefs(void) = 0;

  /* void savePrefFile (in nsIFile file); */
  NS_IMETHOD SavePrefFile(nsIFile *file) = 0;

  /* nsIPrefBranch getBranch (in string pref_root); */
  NS_IMETHOD GetBranch(const char *pref_root, nsIPrefBranch **_retval) = 0;

  /* nsIPrefBranch getDefaultBranch (in string pref_root); */
  NS_IMETHOD GetDefaultBranch(const char *pref_root, nsIPrefBranch **_retval) = 0;

  enum { ePrefInvalid = 0 };

  enum { ePrefLocked = 1 };

  enum { ePrefUserset = 2 };

  enum { ePrefConfig = 4 };

  enum { ePrefRemote = 8 };

  enum { ePrefLilocal = 16 };

  enum { ePrefString = 32 };

  enum { ePrefInt = 64 };

  enum { ePrefBool = 128 };

  enum { ePrefValuetypeMask = 224 };

  /* readonly attribute string root; */
  NS_IMETHOD GetRoot(char * *aRoot) = 0;

  /* long GetPrefType (in string pref_name); */
  NS_IMETHOD GetPrefType(const char *pref_name, PRInt32 *_retval) = 0;

  /* boolean GetBoolPref (in string pref_name); */
  NS_IMETHOD GetBoolPref(const char *pref_name, PRBool *_retval) = 0;

  /* void SetBoolPref (in string pref_name, in long value); */
  NS_IMETHOD SetBoolPref(const char *pref_name, PRInt32 value) = 0;

  /* string GetCharPref (in string pref_name); */
  NS_IMETHOD GetCharPref(const char *pref_name, char **_retval) = 0;

  /* void SetCharPref (in string pref_name, in string value); */
  NS_IMETHOD SetCharPref(const char *pref_name, const char *value) = 0;

  /* long GetIntPref (in string pref_name); */
  NS_IMETHOD GetIntPref(const char *pref_name, PRInt32 *_retval) = 0;

  /* void SetIntPref (in string pref_name, in long value); */
  NS_IMETHOD SetIntPref(const char *pref_name, PRInt32 value) = 0;

  /* void getComplexValue (in string pref_name, in nsIIDRef type, [iid_is (type), retval] out nsQIResult value); */
  NS_IMETHOD GetComplexValue(const char *pref_name, const nsIID & type, void * *value) = 0;

  /* void setComplexValue (in string pref_name, in nsIIDRef type, in nsISupports value); */
  NS_IMETHOD SetComplexValue(const char *pref_name, const nsIID & type, nsISupports *value) = 0;

  /* void ClearUserPref (in string pref_name); */
  NS_IMETHOD ClearUserPref(const char *pref_name) = 0;

  /* boolean PrefIsLocked (in string pref_name); */
  NS_IMETHOD PrefIsLocked(const char *pref_name, PRBool *_retval) = 0;

  /* void lockPref (in string pref_name); */
  NS_IMETHOD LockPref(const char *pref_name) = 0;

  /* void unlockPref (in string pref_name); */
  NS_IMETHOD UnlockPref(const char *pref_name) = 0;

  /* void resetBranch (in string starting_at); */
  NS_IMETHOD ResetBranch(const char *starting_at) = 0;

  /* void DeleteBranch (in string starting_at); */
  NS_IMETHOD DeleteBranch(const char *starting_at) = 0;

  /* void getChildList (in string starting_at, out unsigned long count, [array, size_is (count), retval] out string child_array); */
  NS_IMETHOD GetChildList(const char *starting_at, PRUint32 *count, char ***child_array) = 0;

  /* void addObserver (in string domain, in nsIObserver observer, in boolean hold_weak); */
  NS_IMETHOD AddObserver(const char *domain, nsIObserver *observer, PRBool hold_weak) = 0;

  /* void removeObserver (in string domain, in nsIObserver observer); */
  NS_IMETHOD RemoveObserver(const char *domain, nsIObserver *observer) = 0;

  /* string CopyCharPref (in string pref); */
  NS_IMETHOD CopyCharPref(const char *pref, char **_retval) = 0;

  /* string CopyDefaultCharPref (in string pref); */
  NS_IMETHOD CopyDefaultCharPref(const char *pref, char **_retval) = 0;

  /* boolean GetDefaultBoolPref (in string pref); */
  NS_IMETHOD GetDefaultBoolPref(const char *pref, PRBool *_retval) = 0;

  /* long GetDefaultIntPref (in string pref); */
  NS_IMETHOD GetDefaultIntPref(const char *pref, PRInt32 *_retval) = 0;

  /* void SetDefaultBoolPref (in string pref, in boolean value); */
  NS_IMETHOD SetDefaultBoolPref(const char *pref, PRBool value) = 0;

  /* void SetDefaultCharPref (in string pref, in string value); */
  NS_IMETHOD SetDefaultCharPref(const char *pref, const char *value) = 0;

  /* void SetDefaultIntPref (in string pref, in long value); */
  NS_IMETHOD SetDefaultIntPref(const char *pref, PRInt32 value) = 0;

  /* wstring CopyUnicharPref (in string pref); */
  NS_IMETHOD CopyUnicharPref(const char *pref, PRUnichar **_retval) = 0;

  /* wstring CopyDefaultUnicharPref (in string pref); */
  NS_IMETHOD CopyDefaultUnicharPref(const char *pref, PRUnichar **_retval) = 0;

  /* void SetUnicharPref (in string pref, in wstring value); */
  NS_IMETHOD SetUnicharPref(const char *pref, const PRUnichar *value) = 0;

  /* void SetDefaultUnicharPref (in string pref, in wstring value); */
  NS_IMETHOD SetDefaultUnicharPref(const char *pref, const PRUnichar *value) = 0;

  /* wstring getDefaultLocalizedUnicharPref (in string pref); */
  NS_IMETHOD GetDefaultLocalizedUnicharPref(const char *pref, PRUnichar **_retval) = 0;

  /* wstring getLocalizedUnicharPref (in string pref); */
  NS_IMETHOD GetLocalizedUnicharPref(const char *pref, PRUnichar **_retval) = 0;

  /* nsIFileSpec GetFilePref (in string pref); */
  NS_IMETHOD GetFilePref(const char *pref, nsIFileSpec **_retval) = 0;

  /* void SetFilePref (in string pref, in nsIFileSpec value, in boolean set_default); */
  NS_IMETHOD SetFilePref(const char *pref, nsIFileSpec *value, PRBool set_default) = 0;

  /* nsILocalFile getFileXPref (in string pref); */
  NS_IMETHOD GetFileXPref(const char *pref, nsILocalFile **_retval) = 0;

  /* void setFileXPref (in string pref, in nsILocalFile value); */
  NS_IMETHOD SetFileXPref(const char *pref, nsILocalFile *value) = 0;

  /* [noscript] void RegisterCallback (in string domain, in PrefChangedFunc callback, in voidPtr closure); */
  NS_IMETHOD RegisterCallback(const char *domain, PrefChangedFunc callback, void * closure) = 0;

  /* [noscript] void UnregisterCallback (in string domain, in PrefChangedFunc callback, in voidPtr closure); */
  NS_IMETHOD UnregisterCallback(const char *domain, PrefChangedFunc callback, void * closure) = 0;

  /* [noscript] void EnumerateChildren (in string parent, in PrefEnumerationFunc callback, in voidPtr data); */
  NS_IMETHOD EnumerateChildren(const char *parent, PrefEnumerationFunc callback, void * data) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPREF \
  NS_IMETHOD ReadUserPrefs(nsIFile *file); \
  NS_IMETHOD ResetPrefs(void); \
  NS_IMETHOD ResetUserPrefs(void); \
  NS_IMETHOD SavePrefFile(nsIFile *file); \
  NS_IMETHOD GetBranch(const char *pref_root, nsIPrefBranch **_retval); \
  NS_IMETHOD GetDefaultBranch(const char *pref_root, nsIPrefBranch **_retval); \
  NS_IMETHOD GetRoot(char * *aRoot); \
  NS_IMETHOD GetPrefType(const char *pref_name, PRInt32 *_retval); \
  NS_IMETHOD GetBoolPref(const char *pref_name, PRBool *_retval); \
  NS_IMETHOD SetBoolPref(const char *pref_name, PRInt32 value); \
  NS_IMETHOD GetCharPref(const char *pref_name, char **_retval); \
  NS_IMETHOD SetCharPref(const char *pref_name, const char *value); \
  NS_IMETHOD GetIntPref(const char *pref_name, PRInt32 *_retval); \
  NS_IMETHOD SetIntPref(const char *pref_name, PRInt32 value); \
  NS_IMETHOD GetComplexValue(const char *pref_name, const nsIID & type, void * *value); \
  NS_IMETHOD SetComplexValue(const char *pref_name, const nsIID & type, nsISupports *value); \
  NS_IMETHOD ClearUserPref(const char *pref_name); \
  NS_IMETHOD PrefIsLocked(const char *pref_name, PRBool *_retval); \
  NS_IMETHOD LockPref(const char *pref_name); \
  NS_IMETHOD UnlockPref(const char *pref_name); \
  NS_IMETHOD ResetBranch(const char *starting_at); \
  NS_IMETHOD DeleteBranch(const char *starting_at); \
  NS_IMETHOD GetChildList(const char *starting_at, PRUint32 *count, char ***child_array); \
  NS_IMETHOD AddObserver(const char *domain, nsIObserver *observer, PRBool hold_weak); \
  NS_IMETHOD RemoveObserver(const char *domain, nsIObserver *observer); \
  NS_IMETHOD CopyCharPref(const char *pref, char **_retval); \
  NS_IMETHOD CopyDefaultCharPref(const char *pref, char **_retval); \
  NS_IMETHOD GetDefaultBoolPref(const char *pref, PRBool *_retval); \
  NS_IMETHOD GetDefaultIntPref(const char *pref, PRInt32 *_retval); \
  NS_IMETHOD SetDefaultBoolPref(const char *pref, PRBool value); \
  NS_IMETHOD SetDefaultCharPref(const char *pref, const char *value); \
  NS_IMETHOD SetDefaultIntPref(const char *pref, PRInt32 value); \
  NS_IMETHOD CopyUnicharPref(const char *pref, PRUnichar **_retval); \
  NS_IMETHOD CopyDefaultUnicharPref(const char *pref, PRUnichar **_retval); \
  NS_IMETHOD SetUnicharPref(const char *pref, const PRUnichar *value); \
  NS_IMETHOD SetDefaultUnicharPref(const char *pref, const PRUnichar *value); \
  NS_IMETHOD GetDefaultLocalizedUnicharPref(const char *pref, PRUnichar **_retval); \
  NS_IMETHOD GetLocalizedUnicharPref(const char *pref, PRUnichar **_retval); \
  NS_IMETHOD GetFilePref(const char *pref, nsIFileSpec **_retval); \
  NS_IMETHOD SetFilePref(const char *pref, nsIFileSpec *value, PRBool set_default); \
  NS_IMETHOD GetFileXPref(const char *pref, nsILocalFile **_retval); \
  NS_IMETHOD SetFileXPref(const char *pref, nsILocalFile *value); \
  NS_IMETHOD RegisterCallback(const char *domain, PrefChangedFunc callback, void * closure); \
  NS_IMETHOD UnregisterCallback(const char *domain, PrefChangedFunc callback, void * closure); \
  NS_IMETHOD EnumerateChildren(const char *parent, PrefEnumerationFunc callback, void * data); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPREF(_to) \
  NS_IMETHOD ReadUserPrefs(nsIFile *file) { return _to ReadUserPrefs(file); } \
  NS_IMETHOD ResetPrefs(void) { return _to ResetPrefs(); } \
  NS_IMETHOD ResetUserPrefs(void) { return _to ResetUserPrefs(); } \
  NS_IMETHOD SavePrefFile(nsIFile *file) { return _to SavePrefFile(file); } \
  NS_IMETHOD GetBranch(const char *pref_root, nsIPrefBranch **_retval) { return _to GetBranch(pref_root, _retval); } \
  NS_IMETHOD GetDefaultBranch(const char *pref_root, nsIPrefBranch **_retval) { return _to GetDefaultBranch(pref_root, _retval); } \
  NS_IMETHOD GetRoot(char * *aRoot) { return _to GetRoot(aRoot); } \
  NS_IMETHOD GetPrefType(const char *pref_name, PRInt32 *_retval) { return _to GetPrefType(pref_name, _retval); } \
  NS_IMETHOD GetBoolPref(const char *pref_name, PRBool *_retval) { return _to GetBoolPref(pref_name, _retval); } \
  NS_IMETHOD SetBoolPref(const char *pref_name, PRInt32 value) { return _to SetBoolPref(pref_name, value); } \
  NS_IMETHOD GetCharPref(const char *pref_name, char **_retval) { return _to GetCharPref(pref_name, _retval); } \
  NS_IMETHOD SetCharPref(const char *pref_name, const char *value) { return _to SetCharPref(pref_name, value); } \
  NS_IMETHOD GetIntPref(const char *pref_name, PRInt32 *_retval) { return _to GetIntPref(pref_name, _retval); } \
  NS_IMETHOD SetIntPref(const char *pref_name, PRInt32 value) { return _to SetIntPref(pref_name, value); } \
  NS_IMETHOD GetComplexValue(const char *pref_name, const nsIID & type, void * *value) { return _to GetComplexValue(pref_name, type, value); } \
  NS_IMETHOD SetComplexValue(const char *pref_name, const nsIID & type, nsISupports *value) { return _to SetComplexValue(pref_name, type, value); } \
  NS_IMETHOD ClearUserPref(const char *pref_name) { return _to ClearUserPref(pref_name); } \
  NS_IMETHOD PrefIsLocked(const char *pref_name, PRBool *_retval) { return _to PrefIsLocked(pref_name, _retval); } \
  NS_IMETHOD LockPref(const char *pref_name) { return _to LockPref(pref_name); } \
  NS_IMETHOD UnlockPref(const char *pref_name) { return _to UnlockPref(pref_name); } \
  NS_IMETHOD ResetBranch(const char *starting_at) { return _to ResetBranch(starting_at); } \
  NS_IMETHOD DeleteBranch(const char *starting_at) { return _to DeleteBranch(starting_at); } \
  NS_IMETHOD GetChildList(const char *starting_at, PRUint32 *count, char ***child_array) { return _to GetChildList(starting_at, count, child_array); } \
  NS_IMETHOD AddObserver(const char *domain, nsIObserver *observer, PRBool hold_weak) { return _to AddObserver(domain, observer, hold_weak); } \
  NS_IMETHOD RemoveObserver(const char *domain, nsIObserver *observer) { return _to RemoveObserver(domain, observer); } \
  NS_IMETHOD CopyCharPref(const char *pref, char **_retval) { return _to CopyCharPref(pref, _retval); } \
  NS_IMETHOD CopyDefaultCharPref(const char *pref, char **_retval) { return _to CopyDefaultCharPref(pref, _retval); } \
  NS_IMETHOD GetDefaultBoolPref(const char *pref, PRBool *_retval) { return _to GetDefaultBoolPref(pref, _retval); } \
  NS_IMETHOD GetDefaultIntPref(const char *pref, PRInt32 *_retval) { return _to GetDefaultIntPref(pref, _retval); } \
  NS_IMETHOD SetDefaultBoolPref(const char *pref, PRBool value) { return _to SetDefaultBoolPref(pref, value); } \
  NS_IMETHOD SetDefaultCharPref(const char *pref, const char *value) { return _to SetDefaultCharPref(pref, value); } \
  NS_IMETHOD SetDefaultIntPref(const char *pref, PRInt32 value) { return _to SetDefaultIntPref(pref, value); } \
  NS_IMETHOD CopyUnicharPref(const char *pref, PRUnichar **_retval) { return _to CopyUnicharPref(pref, _retval); } \
  NS_IMETHOD CopyDefaultUnicharPref(const char *pref, PRUnichar **_retval) { return _to CopyDefaultUnicharPref(pref, _retval); } \
  NS_IMETHOD SetUnicharPref(const char *pref, const PRUnichar *value) { return _to SetUnicharPref(pref, value); } \
  NS_IMETHOD SetDefaultUnicharPref(const char *pref, const PRUnichar *value) { return _to SetDefaultUnicharPref(pref, value); } \
  NS_IMETHOD GetDefaultLocalizedUnicharPref(const char *pref, PRUnichar **_retval) { return _to GetDefaultLocalizedUnicharPref(pref, _retval); } \
  NS_IMETHOD GetLocalizedUnicharPref(const char *pref, PRUnichar **_retval) { return _to GetLocalizedUnicharPref(pref, _retval); } \
  NS_IMETHOD GetFilePref(const char *pref, nsIFileSpec **_retval) { return _to GetFilePref(pref, _retval); } \
  NS_IMETHOD SetFilePref(const char *pref, nsIFileSpec *value, PRBool set_default) { return _to SetFilePref(pref, value, set_default); } \
  NS_IMETHOD GetFileXPref(const char *pref, nsILocalFile **_retval) { return _to GetFileXPref(pref, _retval); } \
  NS_IMETHOD SetFileXPref(const char *pref, nsILocalFile *value) { return _to SetFileXPref(pref, value); } \
  NS_IMETHOD RegisterCallback(const char *domain, PrefChangedFunc callback, void * closure) { return _to RegisterCallback(domain, callback, closure); } \
  NS_IMETHOD UnregisterCallback(const char *domain, PrefChangedFunc callback, void * closure) { return _to UnregisterCallback(domain, callback, closure); } \
  NS_IMETHOD EnumerateChildren(const char *parent, PrefEnumerationFunc callback, void * data) { return _to EnumerateChildren(parent, callback, data); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPREF(_to) \
  NS_IMETHOD ReadUserPrefs(nsIFile *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->ReadUserPrefs(file); } \
  NS_IMETHOD ResetPrefs(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ResetPrefs(); } \
  NS_IMETHOD ResetUserPrefs(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ResetUserPrefs(); } \
  NS_IMETHOD SavePrefFile(nsIFile *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->SavePrefFile(file); } \
  NS_IMETHOD GetBranch(const char *pref_root, nsIPrefBranch **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBranch(pref_root, _retval); } \
  NS_IMETHOD GetDefaultBranch(const char *pref_root, nsIPrefBranch **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultBranch(pref_root, _retval); } \
  NS_IMETHOD GetRoot(char * *aRoot) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRoot(aRoot); } \
  NS_IMETHOD GetPrefType(const char *pref_name, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrefType(pref_name, _retval); } \
  NS_IMETHOD GetBoolPref(const char *pref_name, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBoolPref(pref_name, _retval); } \
  NS_IMETHOD SetBoolPref(const char *pref_name, PRInt32 value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetBoolPref(pref_name, value); } \
  NS_IMETHOD GetCharPref(const char *pref_name, char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCharPref(pref_name, _retval); } \
  NS_IMETHOD SetCharPref(const char *pref_name, const char *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetCharPref(pref_name, value); } \
  NS_IMETHOD GetIntPref(const char *pref_name, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIntPref(pref_name, _retval); } \
  NS_IMETHOD SetIntPref(const char *pref_name, PRInt32 value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetIntPref(pref_name, value); } \
  NS_IMETHOD GetComplexValue(const char *pref_name, const nsIID & type, void * *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetComplexValue(pref_name, type, value); } \
  NS_IMETHOD SetComplexValue(const char *pref_name, const nsIID & type, nsISupports *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetComplexValue(pref_name, type, value); } \
  NS_IMETHOD ClearUserPref(const char *pref_name) { return !_to ? NS_ERROR_NULL_POINTER : _to->ClearUserPref(pref_name); } \
  NS_IMETHOD PrefIsLocked(const char *pref_name, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->PrefIsLocked(pref_name, _retval); } \
  NS_IMETHOD LockPref(const char *pref_name) { return !_to ? NS_ERROR_NULL_POINTER : _to->LockPref(pref_name); } \
  NS_IMETHOD UnlockPref(const char *pref_name) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnlockPref(pref_name); } \
  NS_IMETHOD ResetBranch(const char *starting_at) { return !_to ? NS_ERROR_NULL_POINTER : _to->ResetBranch(starting_at); } \
  NS_IMETHOD DeleteBranch(const char *starting_at) { return !_to ? NS_ERROR_NULL_POINTER : _to->DeleteBranch(starting_at); } \
  NS_IMETHOD GetChildList(const char *starting_at, PRUint32 *count, char ***child_array) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChildList(starting_at, count, child_array); } \
  NS_IMETHOD AddObserver(const char *domain, nsIObserver *observer, PRBool hold_weak) { return !_to ? NS_ERROR_NULL_POINTER : _to->AddObserver(domain, observer, hold_weak); } \
  NS_IMETHOD RemoveObserver(const char *domain, nsIObserver *observer) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveObserver(domain, observer); } \
  NS_IMETHOD CopyCharPref(const char *pref, char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyCharPref(pref, _retval); } \
  NS_IMETHOD CopyDefaultCharPref(const char *pref, char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyDefaultCharPref(pref, _retval); } \
  NS_IMETHOD GetDefaultBoolPref(const char *pref, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultBoolPref(pref, _retval); } \
  NS_IMETHOD GetDefaultIntPref(const char *pref, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultIntPref(pref, _retval); } \
  NS_IMETHOD SetDefaultBoolPref(const char *pref, PRBool value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultBoolPref(pref, value); } \
  NS_IMETHOD SetDefaultCharPref(const char *pref, const char *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultCharPref(pref, value); } \
  NS_IMETHOD SetDefaultIntPref(const char *pref, PRInt32 value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultIntPref(pref, value); } \
  NS_IMETHOD CopyUnicharPref(const char *pref, PRUnichar **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyUnicharPref(pref, _retval); } \
  NS_IMETHOD CopyDefaultUnicharPref(const char *pref, PRUnichar **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CopyDefaultUnicharPref(pref, _retval); } \
  NS_IMETHOD SetUnicharPref(const char *pref, const PRUnichar *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetUnicharPref(pref, value); } \
  NS_IMETHOD SetDefaultUnicharPref(const char *pref, const PRUnichar *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDefaultUnicharPref(pref, value); } \
  NS_IMETHOD GetDefaultLocalizedUnicharPref(const char *pref, PRUnichar **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDefaultLocalizedUnicharPref(pref, _retval); } \
  NS_IMETHOD GetLocalizedUnicharPref(const char *pref, PRUnichar **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLocalizedUnicharPref(pref, _retval); } \
  NS_IMETHOD GetFilePref(const char *pref, nsIFileSpec **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFilePref(pref, _retval); } \
  NS_IMETHOD SetFilePref(const char *pref, nsIFileSpec *value, PRBool set_default) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFilePref(pref, value, set_default); } \
  NS_IMETHOD GetFileXPref(const char *pref, nsILocalFile **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFileXPref(pref, _retval); } \
  NS_IMETHOD SetFileXPref(const char *pref, nsILocalFile *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFileXPref(pref, value); } \
  NS_IMETHOD RegisterCallback(const char *domain, PrefChangedFunc callback, void * closure) { return !_to ? NS_ERROR_NULL_POINTER : _to->RegisterCallback(domain, callback, closure); } \
  NS_IMETHOD UnregisterCallback(const char *domain, PrefChangedFunc callback, void * closure) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnregisterCallback(domain, callback, closure); } \
  NS_IMETHOD EnumerateChildren(const char *parent, PrefEnumerationFunc callback, void * data) { return !_to ? NS_ERROR_NULL_POINTER : _to->EnumerateChildren(parent, callback, data); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsPref : public nsIPref
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPREF

  nsPref();

private:
  ~nsPref();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsPref, nsIPref)

nsPref::nsPref()
{
  /* member initializers and constructor code */
}

nsPref::~nsPref()
{
  /* destructor code */
}

/* void readUserPrefs (in nsIFile file); */
NS_IMETHODIMP nsPref::ReadUserPrefs(nsIFile *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void ResetPrefs (); */
NS_IMETHODIMP nsPref::ResetPrefs()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void ResetUserPrefs (); */
NS_IMETHODIMP nsPref::ResetUserPrefs()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void savePrefFile (in nsIFile file); */
NS_IMETHODIMP nsPref::SavePrefFile(nsIFile *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrefBranch getBranch (in string pref_root); */
NS_IMETHODIMP nsPref::GetBranch(const char *pref_root, nsIPrefBranch **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrefBranch getDefaultBranch (in string pref_root); */
NS_IMETHODIMP nsPref::GetDefaultBranch(const char *pref_root, nsIPrefBranch **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute string root; */
NS_IMETHODIMP nsPref::GetRoot(char * *aRoot)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long GetPrefType (in string pref_name); */
NS_IMETHODIMP nsPref::GetPrefType(const char *pref_name, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean GetBoolPref (in string pref_name); */
NS_IMETHODIMP nsPref::GetBoolPref(const char *pref_name, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetBoolPref (in string pref_name, in long value); */
NS_IMETHODIMP nsPref::SetBoolPref(const char *pref_name, PRInt32 value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string GetCharPref (in string pref_name); */
NS_IMETHODIMP nsPref::GetCharPref(const char *pref_name, char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetCharPref (in string pref_name, in string value); */
NS_IMETHODIMP nsPref::SetCharPref(const char *pref_name, const char *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long GetIntPref (in string pref_name); */
NS_IMETHODIMP nsPref::GetIntPref(const char *pref_name, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetIntPref (in string pref_name, in long value); */
NS_IMETHODIMP nsPref::SetIntPref(const char *pref_name, PRInt32 value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getComplexValue (in string pref_name, in nsIIDRef type, [iid_is (type), retval] out nsQIResult value); */
NS_IMETHODIMP nsPref::GetComplexValue(const char *pref_name, const nsIID & type, void * *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setComplexValue (in string pref_name, in nsIIDRef type, in nsISupports value); */
NS_IMETHODIMP nsPref::SetComplexValue(const char *pref_name, const nsIID & type, nsISupports *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void ClearUserPref (in string pref_name); */
NS_IMETHODIMP nsPref::ClearUserPref(const char *pref_name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean PrefIsLocked (in string pref_name); */
NS_IMETHODIMP nsPref::PrefIsLocked(const char *pref_name, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void lockPref (in string pref_name); */
NS_IMETHODIMP nsPref::LockPref(const char *pref_name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void unlockPref (in string pref_name); */
NS_IMETHODIMP nsPref::UnlockPref(const char *pref_name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void resetBranch (in string starting_at); */
NS_IMETHODIMP nsPref::ResetBranch(const char *starting_at)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void DeleteBranch (in string starting_at); */
NS_IMETHODIMP nsPref::DeleteBranch(const char *starting_at)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getChildList (in string starting_at, out unsigned long count, [array, size_is (count), retval] out string child_array); */
NS_IMETHODIMP nsPref::GetChildList(const char *starting_at, PRUint32 *count, char ***child_array)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void addObserver (in string domain, in nsIObserver observer, in boolean hold_weak); */
NS_IMETHODIMP nsPref::AddObserver(const char *domain, nsIObserver *observer, PRBool hold_weak)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeObserver (in string domain, in nsIObserver observer); */
NS_IMETHODIMP nsPref::RemoveObserver(const char *domain, nsIObserver *observer)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string CopyCharPref (in string pref); */
NS_IMETHODIMP nsPref::CopyCharPref(const char *pref, char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string CopyDefaultCharPref (in string pref); */
NS_IMETHODIMP nsPref::CopyDefaultCharPref(const char *pref, char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean GetDefaultBoolPref (in string pref); */
NS_IMETHODIMP nsPref::GetDefaultBoolPref(const char *pref, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long GetDefaultIntPref (in string pref); */
NS_IMETHODIMP nsPref::GetDefaultIntPref(const char *pref, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetDefaultBoolPref (in string pref, in boolean value); */
NS_IMETHODIMP nsPref::SetDefaultBoolPref(const char *pref, PRBool value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetDefaultCharPref (in string pref, in string value); */
NS_IMETHODIMP nsPref::SetDefaultCharPref(const char *pref, const char *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetDefaultIntPref (in string pref, in long value); */
NS_IMETHODIMP nsPref::SetDefaultIntPref(const char *pref, PRInt32 value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* wstring CopyUnicharPref (in string pref); */
NS_IMETHODIMP nsPref::CopyUnicharPref(const char *pref, PRUnichar **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* wstring CopyDefaultUnicharPref (in string pref); */
NS_IMETHODIMP nsPref::CopyDefaultUnicharPref(const char *pref, PRUnichar **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetUnicharPref (in string pref, in wstring value); */
NS_IMETHODIMP nsPref::SetUnicharPref(const char *pref, const PRUnichar *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetDefaultUnicharPref (in string pref, in wstring value); */
NS_IMETHODIMP nsPref::SetDefaultUnicharPref(const char *pref, const PRUnichar *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* wstring getDefaultLocalizedUnicharPref (in string pref); */
NS_IMETHODIMP nsPref::GetDefaultLocalizedUnicharPref(const char *pref, PRUnichar **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* wstring getLocalizedUnicharPref (in string pref); */
NS_IMETHODIMP nsPref::GetLocalizedUnicharPref(const char *pref, PRUnichar **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIFileSpec GetFilePref (in string pref); */
NS_IMETHODIMP nsPref::GetFilePref(const char *pref, nsIFileSpec **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetFilePref (in string pref, in nsIFileSpec value, in boolean set_default); */
NS_IMETHODIMP nsPref::SetFilePref(const char *pref, nsIFileSpec *value, PRBool set_default)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsILocalFile getFileXPref (in string pref); */
NS_IMETHODIMP nsPref::GetFileXPref(const char *pref, nsILocalFile **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setFileXPref (in string pref, in nsILocalFile value); */
NS_IMETHODIMP nsPref::SetFileXPref(const char *pref, nsILocalFile *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void RegisterCallback (in string domain, in PrefChangedFunc callback, in voidPtr closure); */
NS_IMETHODIMP nsPref::RegisterCallback(const char *domain, PrefChangedFunc callback, void * closure)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void UnregisterCallback (in string domain, in PrefChangedFunc callback, in voidPtr closure); */
NS_IMETHODIMP nsPref::UnregisterCallback(const char *domain, PrefChangedFunc callback, void * closure)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void EnumerateChildren (in string parent, in PrefEnumerationFunc callback, in voidPtr data); */
NS_IMETHODIMP nsPref::EnumerateChildren(const char *parent, PrefEnumerationFunc callback, void * data)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIPrintSettings; /* forward declaration */

class nsIDOMWindow; /* forward declaration */

class nsIWebProgressListener; /* forward declaration */


/* starting interface:    nsIWebBrowserPrint */
#define NS_IWEBBROWSERPRINT_IID_STR "9a7ca4b0-fbba-11d4-a869-00105a183419"

#define NS_IWEBBROWSERPRINT_IID \
  {0x9a7ca4b0, 0xfbba, 0x11d4, \
    { 0xa8, 0x69, 0x00, 0x10, 0x5a, 0x18, 0x34, 0x19 }}

class NS_NO_VTABLE nsIWebBrowserPrint : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBBROWSERPRINT_IID)

  enum { PRINTPREVIEW_GOTO_PAGENUM = 0 };

  enum { PRINTPREVIEW_PREV_PAGE = 1 };

  enum { PRINTPREVIEW_NEXT_PAGE = 2 };

  enum { PRINTPREVIEW_HOME = 3 };

  enum { PRINTPREVIEW_END = 4 };

  /* readonly attribute nsIPrintSettings globalPrintSettings; */
  NS_IMETHOD GetGlobalPrintSettings(nsIPrintSettings * *aGlobalPrintSettings) = 0;

  /* readonly attribute nsIPrintSettings currentPrintSettings; */
  NS_IMETHOD GetCurrentPrintSettings(nsIPrintSettings * *aCurrentPrintSettings) = 0;

  /* readonly attribute nsIDOMWindow currentChildDOMWindow; */
  NS_IMETHOD GetCurrentChildDOMWindow(nsIDOMWindow * *aCurrentChildDOMWindow) = 0;

  /* readonly attribute boolean doingPrint; */
  NS_IMETHOD GetDoingPrint(PRBool *aDoingPrint) = 0;

  /* readonly attribute boolean doingPrintPreview; */
  NS_IMETHOD GetDoingPrintPreview(PRBool *aDoingPrintPreview) = 0;

  /* readonly attribute boolean isFramesetDocument; */
  NS_IMETHOD GetIsFramesetDocument(PRBool *aIsFramesetDocument) = 0;

  /* readonly attribute boolean isFramesetFrameSelected; */
  NS_IMETHOD GetIsFramesetFrameSelected(PRBool *aIsFramesetFrameSelected) = 0;

  /* readonly attribute boolean isIFrameSelected; */
  NS_IMETHOD GetIsIFrameSelected(PRBool *aIsIFrameSelected) = 0;

  /* readonly attribute boolean isRangeSelection; */
  NS_IMETHOD GetIsRangeSelection(PRBool *aIsRangeSelection) = 0;

  /* readonly attribute long printPreviewNumPages; */
  NS_IMETHOD GetPrintPreviewNumPages(PRInt32 *aPrintPreviewNumPages) = 0;

  /* void print (in nsIPrintSettings thePrintSettings, in nsIWebProgressListener WPListener); */
  NS_IMETHOD Print(nsIPrintSettings *thePrintSettings, nsIWebProgressListener *WPListener) = 0;

  /* void printPreview (in nsIPrintSettings thePrintSettings, in nsIDOMWindow childDOMWin, in nsIWebProgressListener WPListener); */
  NS_IMETHOD PrintPreview(nsIPrintSettings *thePrintSettings, nsIDOMWindow *childDOMWin, nsIWebProgressListener *WPListener) = 0;

  /* void printPreviewNavigate (in short navType, in long pageNum); */
  NS_IMETHOD PrintPreviewNavigate(PRInt16 navType, PRInt32 pageNum) = 0;

  /* void cancel (); */
  NS_IMETHOD Cancel(void) = 0;

  /* void enumerateDocumentNames (out PRUint32 count, [array, size_is (count), retval] out wstring result); */
  NS_IMETHOD EnumerateDocumentNames(PRUint32 *count, PRUnichar ***result) = 0;

  /* void exitPrintPreview (); */
  NS_IMETHOD ExitPrintPreview(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBBROWSERPRINT \
  NS_IMETHOD GetGlobalPrintSettings(nsIPrintSettings * *aGlobalPrintSettings); \
  NS_IMETHOD GetCurrentPrintSettings(nsIPrintSettings * *aCurrentPrintSettings); \
  NS_IMETHOD GetCurrentChildDOMWindow(nsIDOMWindow * *aCurrentChildDOMWindow); \
  NS_IMETHOD GetDoingPrint(PRBool *aDoingPrint); \
  NS_IMETHOD GetDoingPrintPreview(PRBool *aDoingPrintPreview); \
  NS_IMETHOD GetIsFramesetDocument(PRBool *aIsFramesetDocument); \
  NS_IMETHOD GetIsFramesetFrameSelected(PRBool *aIsFramesetFrameSelected); \
  NS_IMETHOD GetIsIFrameSelected(PRBool *aIsIFrameSelected); \
  NS_IMETHOD GetIsRangeSelection(PRBool *aIsRangeSelection); \
  NS_IMETHOD GetPrintPreviewNumPages(PRInt32 *aPrintPreviewNumPages); \
  NS_IMETHOD Print(nsIPrintSettings *thePrintSettings, nsIWebProgressListener *WPListener); \
  NS_IMETHOD PrintPreview(nsIPrintSettings *thePrintSettings, nsIDOMWindow *childDOMWin, nsIWebProgressListener *WPListener); \
  NS_IMETHOD PrintPreviewNavigate(PRInt16 navType, PRInt32 pageNum); \
  NS_IMETHOD Cancel(void); \
  NS_IMETHOD EnumerateDocumentNames(PRUint32 *count, PRUnichar ***result); \
  NS_IMETHOD ExitPrintPreview(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBBROWSERPRINT(_to) \
  NS_IMETHOD GetGlobalPrintSettings(nsIPrintSettings * *aGlobalPrintSettings) { return _to GetGlobalPrintSettings(aGlobalPrintSettings); } \
  NS_IMETHOD GetCurrentPrintSettings(nsIPrintSettings * *aCurrentPrintSettings) { return _to GetCurrentPrintSettings(aCurrentPrintSettings); } \
  NS_IMETHOD GetCurrentChildDOMWindow(nsIDOMWindow * *aCurrentChildDOMWindow) { return _to GetCurrentChildDOMWindow(aCurrentChildDOMWindow); } \
  NS_IMETHOD GetDoingPrint(PRBool *aDoingPrint) { return _to GetDoingPrint(aDoingPrint); } \
  NS_IMETHOD GetDoingPrintPreview(PRBool *aDoingPrintPreview) { return _to GetDoingPrintPreview(aDoingPrintPreview); } \
  NS_IMETHOD GetIsFramesetDocument(PRBool *aIsFramesetDocument) { return _to GetIsFramesetDocument(aIsFramesetDocument); } \
  NS_IMETHOD GetIsFramesetFrameSelected(PRBool *aIsFramesetFrameSelected) { return _to GetIsFramesetFrameSelected(aIsFramesetFrameSelected); } \
  NS_IMETHOD GetIsIFrameSelected(PRBool *aIsIFrameSelected) { return _to GetIsIFrameSelected(aIsIFrameSelected); } \
  NS_IMETHOD GetIsRangeSelection(PRBool *aIsRangeSelection) { return _to GetIsRangeSelection(aIsRangeSelection); } \
  NS_IMETHOD GetPrintPreviewNumPages(PRInt32 *aPrintPreviewNumPages) { return _to GetPrintPreviewNumPages(aPrintPreviewNumPages); } \
  NS_IMETHOD Print(nsIPrintSettings *thePrintSettings, nsIWebProgressListener *WPListener) { return _to Print(thePrintSettings, WPListener); } \
  NS_IMETHOD PrintPreview(nsIPrintSettings *thePrintSettings, nsIDOMWindow *childDOMWin, nsIWebProgressListener *WPListener) { return _to PrintPreview(thePrintSettings, childDOMWin, WPListener); } \
  NS_IMETHOD PrintPreviewNavigate(PRInt16 navType, PRInt32 pageNum) { return _to PrintPreviewNavigate(navType, pageNum); } \
  NS_IMETHOD Cancel(void) { return _to Cancel(); } \
  NS_IMETHOD EnumerateDocumentNames(PRUint32 *count, PRUnichar ***result) { return _to EnumerateDocumentNames(count, result); } \
  NS_IMETHOD ExitPrintPreview(void) { return _to ExitPrintPreview(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBBROWSERPRINT(_to) \
  NS_IMETHOD GetGlobalPrintSettings(nsIPrintSettings * *aGlobalPrintSettings) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetGlobalPrintSettings(aGlobalPrintSettings); } \
  NS_IMETHOD GetCurrentPrintSettings(nsIPrintSettings * *aCurrentPrintSettings) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCurrentPrintSettings(aCurrentPrintSettings); } \
  NS_IMETHOD GetCurrentChildDOMWindow(nsIDOMWindow * *aCurrentChildDOMWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCurrentChildDOMWindow(aCurrentChildDOMWindow); } \
  NS_IMETHOD GetDoingPrint(PRBool *aDoingPrint) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDoingPrint(aDoingPrint); } \
  NS_IMETHOD GetDoingPrintPreview(PRBool *aDoingPrintPreview) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDoingPrintPreview(aDoingPrintPreview); } \
  NS_IMETHOD GetIsFramesetDocument(PRBool *aIsFramesetDocument) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsFramesetDocument(aIsFramesetDocument); } \
  NS_IMETHOD GetIsFramesetFrameSelected(PRBool *aIsFramesetFrameSelected) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsFramesetFrameSelected(aIsFramesetFrameSelected); } \
  NS_IMETHOD GetIsIFrameSelected(PRBool *aIsIFrameSelected) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsIFrameSelected(aIsIFrameSelected); } \
  NS_IMETHOD GetIsRangeSelection(PRBool *aIsRangeSelection) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsRangeSelection(aIsRangeSelection); } \
  NS_IMETHOD GetPrintPreviewNumPages(PRInt32 *aPrintPreviewNumPages) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintPreviewNumPages(aPrintPreviewNumPages); } \
  NS_IMETHOD Print(nsIPrintSettings *thePrintSettings, nsIWebProgressListener *WPListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->Print(thePrintSettings, WPListener); } \
  NS_IMETHOD PrintPreview(nsIPrintSettings *thePrintSettings, nsIDOMWindow *childDOMWin, nsIWebProgressListener *WPListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->PrintPreview(thePrintSettings, childDOMWin, WPListener); } \
  NS_IMETHOD PrintPreviewNavigate(PRInt16 navType, PRInt32 pageNum) { return !_to ? NS_ERROR_NULL_POINTER : _to->PrintPreviewNavigate(navType, pageNum); } \
  NS_IMETHOD Cancel(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Cancel(); } \
  NS_IMETHOD EnumerateDocumentNames(PRUint32 *count, PRUnichar ***result) { return !_to ? NS_ERROR_NULL_POINTER : _to->EnumerateDocumentNames(count, result); } \
  NS_IMETHOD ExitPrintPreview(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExitPrintPreview(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebBrowserPrint : public nsIWebBrowserPrint
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBBROWSERPRINT

  nsWebBrowserPrint();

private:
  ~nsWebBrowserPrint();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebBrowserPrint, nsIWebBrowserPrint)

nsWebBrowserPrint::nsWebBrowserPrint()
{
  /* member initializers and constructor code */
}

nsWebBrowserPrint::~nsWebBrowserPrint()
{
  /* destructor code */
}

/* readonly attribute nsIPrintSettings globalPrintSettings; */
NS_IMETHODIMP nsWebBrowserPrint::GetGlobalPrintSettings(nsIPrintSettings * *aGlobalPrintSettings)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIPrintSettings currentPrintSettings; */
NS_IMETHODIMP nsWebBrowserPrint::GetCurrentPrintSettings(nsIPrintSettings * *aCurrentPrintSettings)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMWindow currentChildDOMWindow; */
NS_IMETHODIMP nsWebBrowserPrint::GetCurrentChildDOMWindow(nsIDOMWindow * *aCurrentChildDOMWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean doingPrint; */
NS_IMETHODIMP nsWebBrowserPrint::GetDoingPrint(PRBool *aDoingPrint)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean doingPrintPreview; */
NS_IMETHODIMP nsWebBrowserPrint::GetDoingPrintPreview(PRBool *aDoingPrintPreview)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean isFramesetDocument; */
NS_IMETHODIMP nsWebBrowserPrint::GetIsFramesetDocument(PRBool *aIsFramesetDocument)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean isFramesetFrameSelected; */
NS_IMETHODIMP nsWebBrowserPrint::GetIsFramesetFrameSelected(PRBool *aIsFramesetFrameSelected)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean isIFrameSelected; */
NS_IMETHODIMP nsWebBrowserPrint::GetIsIFrameSelected(PRBool *aIsIFrameSelected)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean isRangeSelection; */
NS_IMETHODIMP nsWebBrowserPrint::GetIsRangeSelection(PRBool *aIsRangeSelection)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long printPreviewNumPages; */
NS_IMETHODIMP nsWebBrowserPrint::GetPrintPreviewNumPages(PRInt32 *aPrintPreviewNumPages)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void print (in nsIPrintSettings thePrintSettings, in nsIWebProgressListener WPListener); */
NS_IMETHODIMP nsWebBrowserPrint::Print(nsIPrintSettings *thePrintSettings, nsIWebProgressListener *WPListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void printPreview (in nsIPrintSettings thePrintSettings, in nsIDOMWindow childDOMWin, in nsIWebProgressListener WPListener); */
NS_IMETHODIMP nsWebBrowserPrint::PrintPreview(nsIPrintSettings *thePrintSettings, nsIDOMWindow *childDOMWin, nsIWebProgressListener *WPListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void printPreviewNavigate (in short navType, in long pageNum); */
NS_IMETHODIMP nsWebBrowserPrint::PrintPreviewNavigate(PRInt16 navType, PRInt32 pageNum)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void cancel (); */
NS_IMETHODIMP nsWebBrowserPrint::Cancel()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void enumerateDocumentNames (out PRUint32 count, [array, size_is (count), retval] out wstring result); */
NS_IMETHODIMP nsWebBrowserPrint::EnumerateDocumentNames(PRUint32 *count, PRUnichar ***result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void exitPrintPreview (); */
NS_IMETHODIMP nsWebBrowserPrint::ExitPrintPreview()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIPrintSession; /* forward declaration */


/* starting interface:    nsIPrintSettings */
#define NS_IPRINTSETTINGS_IID_STR "f1094df6-ce0e-42c9-9847-2f663172c38d"

#define NS_IPRINTSETTINGS_IID \
  {0xf1094df6, 0xce0e, 0x42c9, \
    { 0x98, 0x47, 0x2f, 0x66, 0x31, 0x72, 0xc3, 0x8d }}

class NS_NO_VTABLE nsIPrintSettings : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IPRINTSETTINGS_IID)

  enum { kInitSaveOddEvenPages = 1U };

  enum { kInitSaveHeaderLeft = 2U };

  enum { kInitSaveHeaderCenter = 4U };

  enum { kInitSaveHeaderRight = 8U };

  enum { kInitSaveFooterLeft = 16U };

  enum { kInitSaveFooterCenter = 32U };

  enum { kInitSaveFooterRight = 64U };

  enum { kInitSaveBGColors = 128U };

  enum { kInitSaveBGImages = 256U };

  enum { kInitSavePaperSize = 512U };

  enum { kInitSavePaperName = 1024U };

  enum { kInitSavePaperSizeUnit = 2048U };

  enum { kInitSavePaperSizeType = 4096U };

  enum { kInitSavePaperData = 8192U };

  enum { kInitSavePaperWidth = 16384U };

  enum { kInitSavePaperHeight = 32768U };

  enum { kInitSaveReversed = 65536U };

  enum { kInitSaveInColor = 131072U };

  enum { kInitSaveOrientation = 262144U };

  enum { kInitSavePrintCommand = 524288U };

  enum { kInitSavePrinterName = 1048576U };

  enum { kInitSavePrintToFile = 2097152U };

  enum { kInitSaveToFileName = 4194304U };

  enum { kInitSavePageDelay = 8388608U };

  enum { kInitSaveMargins = 16777216U };

  enum { kInitSaveNativeData = 33554432U };

  enum { kInitSavePlexName = 67108864U };

  enum { kInitSaveShrinkToFit = 134217728U };

  enum { kInitSaveScaling = 268435456U };

  enum { kInitSaveColorspace = 536870912U };

  enum { kInitSaveResolutionName = 1073741824U };

  enum { kInitSaveDownloadFonts = 2147483648U };

  enum { kInitSaveAll = 4294967295U };

  enum { kPrintOddPages = 1 };

  enum { kPrintEvenPages = 2 };

  enum { kEnableSelectionRB = 4 };

  enum { kRangeAllPages = 0 };

  enum { kRangeSpecifiedPageRange = 1 };

  enum { kRangeSelection = 2 };

  enum { kRangeFocusFrame = 3 };

  enum { kJustLeft = 0 };

  enum { kJustCenter = 1 };

  enum { kJustRight = 2 };

  enum { kUseInternalDefault = 0 };

  enum { kUseSettingWhenPossible = 1 };

  enum { kPaperSizeNativeData = 0 };

  enum { kPaperSizeDefined = 1 };

  enum { kPaperSizeInches = 0 };

  enum { kPaperSizeMillimeters = 1 };

  enum { kPortraitOrientation = 0 };

  enum { kLandscapeOrientation = 1 };

  enum { kNoFrames = 0 };

  enum { kFramesAsIs = 1 };

  enum { kSelectedFrame = 2 };

  enum { kEachFrameSep = 3 };

  enum { kFrameEnableNone = 0 };

  enum { kFrameEnableAll = 1 };

  enum { kFrameEnableAsIsAndEach = 2 };

  /* void SetPrintOptions (in long type, in boolean turnOnOff); */
  NS_IMETHOD SetPrintOptions(PRInt32 type, PRBool turnOnOff) = 0;

  /* boolean GetPrintOptions (in long type); */
  NS_IMETHOD GetPrintOptions(PRInt32 type, PRBool *_retval) = 0;

  /* long GetPrintOptionsBits (); */
  NS_IMETHOD GetPrintOptionsBits(PRInt32 *_retval) = 0;

  /* void GetPageSizeInTwips (out long width, out long height); */
  NS_IMETHOD GetPageSizeInTwips(PRInt32 *width, PRInt32 *height) = 0;

  /* nsIPrintSettings clone (); */
  NS_IMETHOD Clone(nsIPrintSettings **_retval) = 0;

  /* void assign (in nsIPrintSettings ps); */
  NS_IMETHOD Assign(nsIPrintSettings *ps) = 0;

  /* [noscript] attribute nsIPrintSession printSession; */
  NS_IMETHOD GetPrintSession(nsIPrintSession * *aPrintSession) = 0;
  NS_IMETHOD SetPrintSession(nsIPrintSession * aPrintSession) = 0;

  /* attribute long startPageRange; */
  NS_IMETHOD GetStartPageRange(PRInt32 *aStartPageRange) = 0;
  NS_IMETHOD SetStartPageRange(PRInt32 aStartPageRange) = 0;

  /* attribute long endPageRange; */
  NS_IMETHOD GetEndPageRange(PRInt32 *aEndPageRange) = 0;
  NS_IMETHOD SetEndPageRange(PRInt32 aEndPageRange) = 0;

  /* attribute double marginTop; */
  NS_IMETHOD GetMarginTop(double *aMarginTop) = 0;
  NS_IMETHOD SetMarginTop(double aMarginTop) = 0;

  /* attribute double marginLeft; */
  NS_IMETHOD GetMarginLeft(double *aMarginLeft) = 0;
  NS_IMETHOD SetMarginLeft(double aMarginLeft) = 0;

  /* attribute double marginBottom; */
  NS_IMETHOD GetMarginBottom(double *aMarginBottom) = 0;
  NS_IMETHOD SetMarginBottom(double aMarginBottom) = 0;

  /* attribute double marginRight; */
  NS_IMETHOD GetMarginRight(double *aMarginRight) = 0;
  NS_IMETHOD SetMarginRight(double aMarginRight) = 0;

  /* attribute double scaling; */
  NS_IMETHOD GetScaling(double *aScaling) = 0;
  NS_IMETHOD SetScaling(double aScaling) = 0;

  /* attribute boolean printBGColors; */
  NS_IMETHOD GetPrintBGColors(PRBool *aPrintBGColors) = 0;
  NS_IMETHOD SetPrintBGColors(PRBool aPrintBGColors) = 0;

  /* attribute boolean printBGImages; */
  NS_IMETHOD GetPrintBGImages(PRBool *aPrintBGImages) = 0;
  NS_IMETHOD SetPrintBGImages(PRBool aPrintBGImages) = 0;

  /* attribute short printRange; */
  NS_IMETHOD GetPrintRange(PRInt16 *aPrintRange) = 0;
  NS_IMETHOD SetPrintRange(PRInt16 aPrintRange) = 0;

  /* attribute wstring title; */
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) = 0;
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) = 0;

  /* attribute wstring docURL; */
  NS_IMETHOD GetDocURL(PRUnichar * *aDocURL) = 0;
  NS_IMETHOD SetDocURL(const PRUnichar * aDocURL) = 0;

  /* attribute wstring headerStrLeft; */
  NS_IMETHOD GetHeaderStrLeft(PRUnichar * *aHeaderStrLeft) = 0;
  NS_IMETHOD SetHeaderStrLeft(const PRUnichar * aHeaderStrLeft) = 0;

  /* attribute wstring headerStrCenter; */
  NS_IMETHOD GetHeaderStrCenter(PRUnichar * *aHeaderStrCenter) = 0;
  NS_IMETHOD SetHeaderStrCenter(const PRUnichar * aHeaderStrCenter) = 0;

  /* attribute wstring headerStrRight; */
  NS_IMETHOD GetHeaderStrRight(PRUnichar * *aHeaderStrRight) = 0;
  NS_IMETHOD SetHeaderStrRight(const PRUnichar * aHeaderStrRight) = 0;

  /* attribute wstring footerStrLeft; */
  NS_IMETHOD GetFooterStrLeft(PRUnichar * *aFooterStrLeft) = 0;
  NS_IMETHOD SetFooterStrLeft(const PRUnichar * aFooterStrLeft) = 0;

  /* attribute wstring footerStrCenter; */
  NS_IMETHOD GetFooterStrCenter(PRUnichar * *aFooterStrCenter) = 0;
  NS_IMETHOD SetFooterStrCenter(const PRUnichar * aFooterStrCenter) = 0;

  /* attribute wstring footerStrRight; */
  NS_IMETHOD GetFooterStrRight(PRUnichar * *aFooterStrRight) = 0;
  NS_IMETHOD SetFooterStrRight(const PRUnichar * aFooterStrRight) = 0;

  /* attribute short howToEnableFrameUI; */
  NS_IMETHOD GetHowToEnableFrameUI(PRInt16 *aHowToEnableFrameUI) = 0;
  NS_IMETHOD SetHowToEnableFrameUI(PRInt16 aHowToEnableFrameUI) = 0;

  /* attribute boolean isCancelled; */
  NS_IMETHOD GetIsCancelled(PRBool *aIsCancelled) = 0;
  NS_IMETHOD SetIsCancelled(PRBool aIsCancelled) = 0;

  /* attribute short printFrameTypeUsage; */
  NS_IMETHOD GetPrintFrameTypeUsage(PRInt16 *aPrintFrameTypeUsage) = 0;
  NS_IMETHOD SetPrintFrameTypeUsage(PRInt16 aPrintFrameTypeUsage) = 0;

  /* attribute short printFrameType; */
  NS_IMETHOD GetPrintFrameType(PRInt16 *aPrintFrameType) = 0;
  NS_IMETHOD SetPrintFrameType(PRInt16 aPrintFrameType) = 0;

  /* attribute boolean printSilent; */
  NS_IMETHOD GetPrintSilent(PRBool *aPrintSilent) = 0;
  NS_IMETHOD SetPrintSilent(PRBool aPrintSilent) = 0;

  /* attribute boolean shrinkToFit; */
  NS_IMETHOD GetShrinkToFit(PRBool *aShrinkToFit) = 0;
  NS_IMETHOD SetShrinkToFit(PRBool aShrinkToFit) = 0;

  /* attribute boolean showPrintProgress; */
  NS_IMETHOD GetShowPrintProgress(PRBool *aShowPrintProgress) = 0;
  NS_IMETHOD SetShowPrintProgress(PRBool aShowPrintProgress) = 0;

  /* attribute wstring paperName; */
  NS_IMETHOD GetPaperName(PRUnichar * *aPaperName) = 0;
  NS_IMETHOD SetPaperName(const PRUnichar * aPaperName) = 0;

  /* attribute short paperSizeType; */
  NS_IMETHOD GetPaperSizeType(PRInt16 *aPaperSizeType) = 0;
  NS_IMETHOD SetPaperSizeType(PRInt16 aPaperSizeType) = 0;

  /* attribute short paperData; */
  NS_IMETHOD GetPaperData(PRInt16 *aPaperData) = 0;
  NS_IMETHOD SetPaperData(PRInt16 aPaperData) = 0;

  /* attribute double paperWidth; */
  NS_IMETHOD GetPaperWidth(double *aPaperWidth) = 0;
  NS_IMETHOD SetPaperWidth(double aPaperWidth) = 0;

  /* attribute double paperHeight; */
  NS_IMETHOD GetPaperHeight(double *aPaperHeight) = 0;
  NS_IMETHOD SetPaperHeight(double aPaperHeight) = 0;

  /* attribute short paperSizeUnit; */
  NS_IMETHOD GetPaperSizeUnit(PRInt16 *aPaperSizeUnit) = 0;
  NS_IMETHOD SetPaperSizeUnit(PRInt16 aPaperSizeUnit) = 0;

  /* attribute wstring plexName; */
  NS_IMETHOD GetPlexName(PRUnichar * *aPlexName) = 0;
  NS_IMETHOD SetPlexName(const PRUnichar * aPlexName) = 0;

  /* attribute wstring colorspace; */
  NS_IMETHOD GetColorspace(PRUnichar * *aColorspace) = 0;
  NS_IMETHOD SetColorspace(const PRUnichar * aColorspace) = 0;

  /* attribute wstring resolutionName; */
  NS_IMETHOD GetResolutionName(PRUnichar * *aResolutionName) = 0;
  NS_IMETHOD SetResolutionName(const PRUnichar * aResolutionName) = 0;

  /* attribute boolean downloadFonts; */
  NS_IMETHOD GetDownloadFonts(PRBool *aDownloadFonts) = 0;
  NS_IMETHOD SetDownloadFonts(PRBool aDownloadFonts) = 0;

  /* attribute boolean printReversed; */
  NS_IMETHOD GetPrintReversed(PRBool *aPrintReversed) = 0;
  NS_IMETHOD SetPrintReversed(PRBool aPrintReversed) = 0;

  /* attribute boolean printInColor; */
  NS_IMETHOD GetPrintInColor(PRBool *aPrintInColor) = 0;
  NS_IMETHOD SetPrintInColor(PRBool aPrintInColor) = 0;

  /* attribute long paperSize; */
  NS_IMETHOD GetPaperSize(PRInt32 *aPaperSize) = 0;
  NS_IMETHOD SetPaperSize(PRInt32 aPaperSize) = 0;

  /* attribute long orientation; */
  NS_IMETHOD GetOrientation(PRInt32 *aOrientation) = 0;
  NS_IMETHOD SetOrientation(PRInt32 aOrientation) = 0;

  /* attribute wstring printCommand; */
  NS_IMETHOD GetPrintCommand(PRUnichar * *aPrintCommand) = 0;
  NS_IMETHOD SetPrintCommand(const PRUnichar * aPrintCommand) = 0;

  /* attribute long numCopies; */
  NS_IMETHOD GetNumCopies(PRInt32 *aNumCopies) = 0;
  NS_IMETHOD SetNumCopies(PRInt32 aNumCopies) = 0;

  /* attribute wstring printerName; */
  NS_IMETHOD GetPrinterName(PRUnichar * *aPrinterName) = 0;
  NS_IMETHOD SetPrinterName(const PRUnichar * aPrinterName) = 0;

  /* attribute boolean printToFile; */
  NS_IMETHOD GetPrintToFile(PRBool *aPrintToFile) = 0;
  NS_IMETHOD SetPrintToFile(PRBool aPrintToFile) = 0;

  /* attribute wstring toFileName; */
  NS_IMETHOD GetToFileName(PRUnichar * *aToFileName) = 0;
  NS_IMETHOD SetToFileName(const PRUnichar * aToFileName) = 0;

  /* attribute long printPageDelay; */
  NS_IMETHOD GetPrintPageDelay(PRInt32 *aPrintPageDelay) = 0;
  NS_IMETHOD SetPrintPageDelay(PRInt32 aPrintPageDelay) = 0;

  /* attribute boolean isInitializedFromPrinter; */
  NS_IMETHOD GetIsInitializedFromPrinter(PRBool *aIsInitializedFromPrinter) = 0;
  NS_IMETHOD SetIsInitializedFromPrinter(PRBool aIsInitializedFromPrinter) = 0;

  /* attribute boolean isInitializedFromPrefs; */
  NS_IMETHOD GetIsInitializedFromPrefs(PRBool *aIsInitializedFromPrefs) = 0;
  NS_IMETHOD SetIsInitializedFromPrefs(PRBool aIsInitializedFromPrefs) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPRINTSETTINGS \
  NS_IMETHOD SetPrintOptions(PRInt32 type, PRBool turnOnOff); \
  NS_IMETHOD GetPrintOptions(PRInt32 type, PRBool *_retval); \
  NS_IMETHOD GetPrintOptionsBits(PRInt32 *_retval); \
  NS_IMETHOD GetPageSizeInTwips(PRInt32 *width, PRInt32 *height); \
  NS_IMETHOD Clone(nsIPrintSettings **_retval); \
  NS_IMETHOD Assign(nsIPrintSettings *ps); \
  NS_IMETHOD GetPrintSession(nsIPrintSession * *aPrintSession); \
  NS_IMETHOD SetPrintSession(nsIPrintSession * aPrintSession); \
  NS_IMETHOD GetStartPageRange(PRInt32 *aStartPageRange); \
  NS_IMETHOD SetStartPageRange(PRInt32 aStartPageRange); \
  NS_IMETHOD GetEndPageRange(PRInt32 *aEndPageRange); \
  NS_IMETHOD SetEndPageRange(PRInt32 aEndPageRange); \
  NS_IMETHOD GetMarginTop(double *aMarginTop); \
  NS_IMETHOD SetMarginTop(double aMarginTop); \
  NS_IMETHOD GetMarginLeft(double *aMarginLeft); \
  NS_IMETHOD SetMarginLeft(double aMarginLeft); \
  NS_IMETHOD GetMarginBottom(double *aMarginBottom); \
  NS_IMETHOD SetMarginBottom(double aMarginBottom); \
  NS_IMETHOD GetMarginRight(double *aMarginRight); \
  NS_IMETHOD SetMarginRight(double aMarginRight); \
  NS_IMETHOD GetScaling(double *aScaling); \
  NS_IMETHOD SetScaling(double aScaling); \
  NS_IMETHOD GetPrintBGColors(PRBool *aPrintBGColors); \
  NS_IMETHOD SetPrintBGColors(PRBool aPrintBGColors); \
  NS_IMETHOD GetPrintBGImages(PRBool *aPrintBGImages); \
  NS_IMETHOD SetPrintBGImages(PRBool aPrintBGImages); \
  NS_IMETHOD GetPrintRange(PRInt16 *aPrintRange); \
  NS_IMETHOD SetPrintRange(PRInt16 aPrintRange); \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle); \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle); \
  NS_IMETHOD GetDocURL(PRUnichar * *aDocURL); \
  NS_IMETHOD SetDocURL(const PRUnichar * aDocURL); \
  NS_IMETHOD GetHeaderStrLeft(PRUnichar * *aHeaderStrLeft); \
  NS_IMETHOD SetHeaderStrLeft(const PRUnichar * aHeaderStrLeft); \
  NS_IMETHOD GetHeaderStrCenter(PRUnichar * *aHeaderStrCenter); \
  NS_IMETHOD SetHeaderStrCenter(const PRUnichar * aHeaderStrCenter); \
  NS_IMETHOD GetHeaderStrRight(PRUnichar * *aHeaderStrRight); \
  NS_IMETHOD SetHeaderStrRight(const PRUnichar * aHeaderStrRight); \
  NS_IMETHOD GetFooterStrLeft(PRUnichar * *aFooterStrLeft); \
  NS_IMETHOD SetFooterStrLeft(const PRUnichar * aFooterStrLeft); \
  NS_IMETHOD GetFooterStrCenter(PRUnichar * *aFooterStrCenter); \
  NS_IMETHOD SetFooterStrCenter(const PRUnichar * aFooterStrCenter); \
  NS_IMETHOD GetFooterStrRight(PRUnichar * *aFooterStrRight); \
  NS_IMETHOD SetFooterStrRight(const PRUnichar * aFooterStrRight); \
  NS_IMETHOD GetHowToEnableFrameUI(PRInt16 *aHowToEnableFrameUI); \
  NS_IMETHOD SetHowToEnableFrameUI(PRInt16 aHowToEnableFrameUI); \
  NS_IMETHOD GetIsCancelled(PRBool *aIsCancelled); \
  NS_IMETHOD SetIsCancelled(PRBool aIsCancelled); \
  NS_IMETHOD GetPrintFrameTypeUsage(PRInt16 *aPrintFrameTypeUsage); \
  NS_IMETHOD SetPrintFrameTypeUsage(PRInt16 aPrintFrameTypeUsage); \
  NS_IMETHOD GetPrintFrameType(PRInt16 *aPrintFrameType); \
  NS_IMETHOD SetPrintFrameType(PRInt16 aPrintFrameType); \
  NS_IMETHOD GetPrintSilent(PRBool *aPrintSilent); \
  NS_IMETHOD SetPrintSilent(PRBool aPrintSilent); \
  NS_IMETHOD GetShrinkToFit(PRBool *aShrinkToFit); \
  NS_IMETHOD SetShrinkToFit(PRBool aShrinkToFit); \
  NS_IMETHOD GetShowPrintProgress(PRBool *aShowPrintProgress); \
  NS_IMETHOD SetShowPrintProgress(PRBool aShowPrintProgress); \
  NS_IMETHOD GetPaperName(PRUnichar * *aPaperName); \
  NS_IMETHOD SetPaperName(const PRUnichar * aPaperName); \
  NS_IMETHOD GetPaperSizeType(PRInt16 *aPaperSizeType); \
  NS_IMETHOD SetPaperSizeType(PRInt16 aPaperSizeType); \
  NS_IMETHOD GetPaperData(PRInt16 *aPaperData); \
  NS_IMETHOD SetPaperData(PRInt16 aPaperData); \
  NS_IMETHOD GetPaperWidth(double *aPaperWidth); \
  NS_IMETHOD SetPaperWidth(double aPaperWidth); \
  NS_IMETHOD GetPaperHeight(double *aPaperHeight); \
  NS_IMETHOD SetPaperHeight(double aPaperHeight); \
  NS_IMETHOD GetPaperSizeUnit(PRInt16 *aPaperSizeUnit); \
  NS_IMETHOD SetPaperSizeUnit(PRInt16 aPaperSizeUnit); \
  NS_IMETHOD GetPlexName(PRUnichar * *aPlexName); \
  NS_IMETHOD SetPlexName(const PRUnichar * aPlexName); \
  NS_IMETHOD GetColorspace(PRUnichar * *aColorspace); \
  NS_IMETHOD SetColorspace(const PRUnichar * aColorspace); \
  NS_IMETHOD GetResolutionName(PRUnichar * *aResolutionName); \
  NS_IMETHOD SetResolutionName(const PRUnichar * aResolutionName); \
  NS_IMETHOD GetDownloadFonts(PRBool *aDownloadFonts); \
  NS_IMETHOD SetDownloadFonts(PRBool aDownloadFonts); \
  NS_IMETHOD GetPrintReversed(PRBool *aPrintReversed); \
  NS_IMETHOD SetPrintReversed(PRBool aPrintReversed); \
  NS_IMETHOD GetPrintInColor(PRBool *aPrintInColor); \
  NS_IMETHOD SetPrintInColor(PRBool aPrintInColor); \
  NS_IMETHOD GetPaperSize(PRInt32 *aPaperSize); \
  NS_IMETHOD SetPaperSize(PRInt32 aPaperSize); \
  NS_IMETHOD GetOrientation(PRInt32 *aOrientation); \
  NS_IMETHOD SetOrientation(PRInt32 aOrientation); \
  NS_IMETHOD GetPrintCommand(PRUnichar * *aPrintCommand); \
  NS_IMETHOD SetPrintCommand(const PRUnichar * aPrintCommand); \
  NS_IMETHOD GetNumCopies(PRInt32 *aNumCopies); \
  NS_IMETHOD SetNumCopies(PRInt32 aNumCopies); \
  NS_IMETHOD GetPrinterName(PRUnichar * *aPrinterName); \
  NS_IMETHOD SetPrinterName(const PRUnichar * aPrinterName); \
  NS_IMETHOD GetPrintToFile(PRBool *aPrintToFile); \
  NS_IMETHOD SetPrintToFile(PRBool aPrintToFile); \
  NS_IMETHOD GetToFileName(PRUnichar * *aToFileName); \
  NS_IMETHOD SetToFileName(const PRUnichar * aToFileName); \
  NS_IMETHOD GetPrintPageDelay(PRInt32 *aPrintPageDelay); \
  NS_IMETHOD SetPrintPageDelay(PRInt32 aPrintPageDelay); \
  NS_IMETHOD GetIsInitializedFromPrinter(PRBool *aIsInitializedFromPrinter); \
  NS_IMETHOD SetIsInitializedFromPrinter(PRBool aIsInitializedFromPrinter); \
  NS_IMETHOD GetIsInitializedFromPrefs(PRBool *aIsInitializedFromPrefs); \
  NS_IMETHOD SetIsInitializedFromPrefs(PRBool aIsInitializedFromPrefs); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPRINTSETTINGS(_to) \
  NS_IMETHOD SetPrintOptions(PRInt32 type, PRBool turnOnOff) { return _to SetPrintOptions(type, turnOnOff); } \
  NS_IMETHOD GetPrintOptions(PRInt32 type, PRBool *_retval) { return _to GetPrintOptions(type, _retval); } \
  NS_IMETHOD GetPrintOptionsBits(PRInt32 *_retval) { return _to GetPrintOptionsBits(_retval); } \
  NS_IMETHOD GetPageSizeInTwips(PRInt32 *width, PRInt32 *height) { return _to GetPageSizeInTwips(width, height); } \
  NS_IMETHOD Clone(nsIPrintSettings **_retval) { return _to Clone(_retval); } \
  NS_IMETHOD Assign(nsIPrintSettings *ps) { return _to Assign(ps); } \
  NS_IMETHOD GetPrintSession(nsIPrintSession * *aPrintSession) { return _to GetPrintSession(aPrintSession); } \
  NS_IMETHOD SetPrintSession(nsIPrintSession * aPrintSession) { return _to SetPrintSession(aPrintSession); } \
  NS_IMETHOD GetStartPageRange(PRInt32 *aStartPageRange) { return _to GetStartPageRange(aStartPageRange); } \
  NS_IMETHOD SetStartPageRange(PRInt32 aStartPageRange) { return _to SetStartPageRange(aStartPageRange); } \
  NS_IMETHOD GetEndPageRange(PRInt32 *aEndPageRange) { return _to GetEndPageRange(aEndPageRange); } \
  NS_IMETHOD SetEndPageRange(PRInt32 aEndPageRange) { return _to SetEndPageRange(aEndPageRange); } \
  NS_IMETHOD GetMarginTop(double *aMarginTop) { return _to GetMarginTop(aMarginTop); } \
  NS_IMETHOD SetMarginTop(double aMarginTop) { return _to SetMarginTop(aMarginTop); } \
  NS_IMETHOD GetMarginLeft(double *aMarginLeft) { return _to GetMarginLeft(aMarginLeft); } \
  NS_IMETHOD SetMarginLeft(double aMarginLeft) { return _to SetMarginLeft(aMarginLeft); } \
  NS_IMETHOD GetMarginBottom(double *aMarginBottom) { return _to GetMarginBottom(aMarginBottom); } \
  NS_IMETHOD SetMarginBottom(double aMarginBottom) { return _to SetMarginBottom(aMarginBottom); } \
  NS_IMETHOD GetMarginRight(double *aMarginRight) { return _to GetMarginRight(aMarginRight); } \
  NS_IMETHOD SetMarginRight(double aMarginRight) { return _to SetMarginRight(aMarginRight); } \
  NS_IMETHOD GetScaling(double *aScaling) { return _to GetScaling(aScaling); } \
  NS_IMETHOD SetScaling(double aScaling) { return _to SetScaling(aScaling); } \
  NS_IMETHOD GetPrintBGColors(PRBool *aPrintBGColors) { return _to GetPrintBGColors(aPrintBGColors); } \
  NS_IMETHOD SetPrintBGColors(PRBool aPrintBGColors) { return _to SetPrintBGColors(aPrintBGColors); } \
  NS_IMETHOD GetPrintBGImages(PRBool *aPrintBGImages) { return _to GetPrintBGImages(aPrintBGImages); } \
  NS_IMETHOD SetPrintBGImages(PRBool aPrintBGImages) { return _to SetPrintBGImages(aPrintBGImages); } \
  NS_IMETHOD GetPrintRange(PRInt16 *aPrintRange) { return _to GetPrintRange(aPrintRange); } \
  NS_IMETHOD SetPrintRange(PRInt16 aPrintRange) { return _to SetPrintRange(aPrintRange); } \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) { return _to GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) { return _to SetTitle(aTitle); } \
  NS_IMETHOD GetDocURL(PRUnichar * *aDocURL) { return _to GetDocURL(aDocURL); } \
  NS_IMETHOD SetDocURL(const PRUnichar * aDocURL) { return _to SetDocURL(aDocURL); } \
  NS_IMETHOD GetHeaderStrLeft(PRUnichar * *aHeaderStrLeft) { return _to GetHeaderStrLeft(aHeaderStrLeft); } \
  NS_IMETHOD SetHeaderStrLeft(const PRUnichar * aHeaderStrLeft) { return _to SetHeaderStrLeft(aHeaderStrLeft); } \
  NS_IMETHOD GetHeaderStrCenter(PRUnichar * *aHeaderStrCenter) { return _to GetHeaderStrCenter(aHeaderStrCenter); } \
  NS_IMETHOD SetHeaderStrCenter(const PRUnichar * aHeaderStrCenter) { return _to SetHeaderStrCenter(aHeaderStrCenter); } \
  NS_IMETHOD GetHeaderStrRight(PRUnichar * *aHeaderStrRight) { return _to GetHeaderStrRight(aHeaderStrRight); } \
  NS_IMETHOD SetHeaderStrRight(const PRUnichar * aHeaderStrRight) { return _to SetHeaderStrRight(aHeaderStrRight); } \
  NS_IMETHOD GetFooterStrLeft(PRUnichar * *aFooterStrLeft) { return _to GetFooterStrLeft(aFooterStrLeft); } \
  NS_IMETHOD SetFooterStrLeft(const PRUnichar * aFooterStrLeft) { return _to SetFooterStrLeft(aFooterStrLeft); } \
  NS_IMETHOD GetFooterStrCenter(PRUnichar * *aFooterStrCenter) { return _to GetFooterStrCenter(aFooterStrCenter); } \
  NS_IMETHOD SetFooterStrCenter(const PRUnichar * aFooterStrCenter) { return _to SetFooterStrCenter(aFooterStrCenter); } \
  NS_IMETHOD GetFooterStrRight(PRUnichar * *aFooterStrRight) { return _to GetFooterStrRight(aFooterStrRight); } \
  NS_IMETHOD SetFooterStrRight(const PRUnichar * aFooterStrRight) { return _to SetFooterStrRight(aFooterStrRight); } \
  NS_IMETHOD GetHowToEnableFrameUI(PRInt16 *aHowToEnableFrameUI) { return _to GetHowToEnableFrameUI(aHowToEnableFrameUI); } \
  NS_IMETHOD SetHowToEnableFrameUI(PRInt16 aHowToEnableFrameUI) { return _to SetHowToEnableFrameUI(aHowToEnableFrameUI); } \
  NS_IMETHOD GetIsCancelled(PRBool *aIsCancelled) { return _to GetIsCancelled(aIsCancelled); } \
  NS_IMETHOD SetIsCancelled(PRBool aIsCancelled) { return _to SetIsCancelled(aIsCancelled); } \
  NS_IMETHOD GetPrintFrameTypeUsage(PRInt16 *aPrintFrameTypeUsage) { return _to GetPrintFrameTypeUsage(aPrintFrameTypeUsage); } \
  NS_IMETHOD SetPrintFrameTypeUsage(PRInt16 aPrintFrameTypeUsage) { return _to SetPrintFrameTypeUsage(aPrintFrameTypeUsage); } \
  NS_IMETHOD GetPrintFrameType(PRInt16 *aPrintFrameType) { return _to GetPrintFrameType(aPrintFrameType); } \
  NS_IMETHOD SetPrintFrameType(PRInt16 aPrintFrameType) { return _to SetPrintFrameType(aPrintFrameType); } \
  NS_IMETHOD GetPrintSilent(PRBool *aPrintSilent) { return _to GetPrintSilent(aPrintSilent); } \
  NS_IMETHOD SetPrintSilent(PRBool aPrintSilent) { return _to SetPrintSilent(aPrintSilent); } \
  NS_IMETHOD GetShrinkToFit(PRBool *aShrinkToFit) { return _to GetShrinkToFit(aShrinkToFit); } \
  NS_IMETHOD SetShrinkToFit(PRBool aShrinkToFit) { return _to SetShrinkToFit(aShrinkToFit); } \
  NS_IMETHOD GetShowPrintProgress(PRBool *aShowPrintProgress) { return _to GetShowPrintProgress(aShowPrintProgress); } \
  NS_IMETHOD SetShowPrintProgress(PRBool aShowPrintProgress) { return _to SetShowPrintProgress(aShowPrintProgress); } \
  NS_IMETHOD GetPaperName(PRUnichar * *aPaperName) { return _to GetPaperName(aPaperName); } \
  NS_IMETHOD SetPaperName(const PRUnichar * aPaperName) { return _to SetPaperName(aPaperName); } \
  NS_IMETHOD GetPaperSizeType(PRInt16 *aPaperSizeType) { return _to GetPaperSizeType(aPaperSizeType); } \
  NS_IMETHOD SetPaperSizeType(PRInt16 aPaperSizeType) { return _to SetPaperSizeType(aPaperSizeType); } \
  NS_IMETHOD GetPaperData(PRInt16 *aPaperData) { return _to GetPaperData(aPaperData); } \
  NS_IMETHOD SetPaperData(PRInt16 aPaperData) { return _to SetPaperData(aPaperData); } \
  NS_IMETHOD GetPaperWidth(double *aPaperWidth) { return _to GetPaperWidth(aPaperWidth); } \
  NS_IMETHOD SetPaperWidth(double aPaperWidth) { return _to SetPaperWidth(aPaperWidth); } \
  NS_IMETHOD GetPaperHeight(double *aPaperHeight) { return _to GetPaperHeight(aPaperHeight); } \
  NS_IMETHOD SetPaperHeight(double aPaperHeight) { return _to SetPaperHeight(aPaperHeight); } \
  NS_IMETHOD GetPaperSizeUnit(PRInt16 *aPaperSizeUnit) { return _to GetPaperSizeUnit(aPaperSizeUnit); } \
  NS_IMETHOD SetPaperSizeUnit(PRInt16 aPaperSizeUnit) { return _to SetPaperSizeUnit(aPaperSizeUnit); } \
  NS_IMETHOD GetPlexName(PRUnichar * *aPlexName) { return _to GetPlexName(aPlexName); } \
  NS_IMETHOD SetPlexName(const PRUnichar * aPlexName) { return _to SetPlexName(aPlexName); } \
  NS_IMETHOD GetColorspace(PRUnichar * *aColorspace) { return _to GetColorspace(aColorspace); } \
  NS_IMETHOD SetColorspace(const PRUnichar * aColorspace) { return _to SetColorspace(aColorspace); } \
  NS_IMETHOD GetResolutionName(PRUnichar * *aResolutionName) { return _to GetResolutionName(aResolutionName); } \
  NS_IMETHOD SetResolutionName(const PRUnichar * aResolutionName) { return _to SetResolutionName(aResolutionName); } \
  NS_IMETHOD GetDownloadFonts(PRBool *aDownloadFonts) { return _to GetDownloadFonts(aDownloadFonts); } \
  NS_IMETHOD SetDownloadFonts(PRBool aDownloadFonts) { return _to SetDownloadFonts(aDownloadFonts); } \
  NS_IMETHOD GetPrintReversed(PRBool *aPrintReversed) { return _to GetPrintReversed(aPrintReversed); } \
  NS_IMETHOD SetPrintReversed(PRBool aPrintReversed) { return _to SetPrintReversed(aPrintReversed); } \
  NS_IMETHOD GetPrintInColor(PRBool *aPrintInColor) { return _to GetPrintInColor(aPrintInColor); } \
  NS_IMETHOD SetPrintInColor(PRBool aPrintInColor) { return _to SetPrintInColor(aPrintInColor); } \
  NS_IMETHOD GetPaperSize(PRInt32 *aPaperSize) { return _to GetPaperSize(aPaperSize); } \
  NS_IMETHOD SetPaperSize(PRInt32 aPaperSize) { return _to SetPaperSize(aPaperSize); } \
  NS_IMETHOD GetOrientation(PRInt32 *aOrientation) { return _to GetOrientation(aOrientation); } \
  NS_IMETHOD SetOrientation(PRInt32 aOrientation) { return _to SetOrientation(aOrientation); } \
  NS_IMETHOD GetPrintCommand(PRUnichar * *aPrintCommand) { return _to GetPrintCommand(aPrintCommand); } \
  NS_IMETHOD SetPrintCommand(const PRUnichar * aPrintCommand) { return _to SetPrintCommand(aPrintCommand); } \
  NS_IMETHOD GetNumCopies(PRInt32 *aNumCopies) { return _to GetNumCopies(aNumCopies); } \
  NS_IMETHOD SetNumCopies(PRInt32 aNumCopies) { return _to SetNumCopies(aNumCopies); } \
  NS_IMETHOD GetPrinterName(PRUnichar * *aPrinterName) { return _to GetPrinterName(aPrinterName); } \
  NS_IMETHOD SetPrinterName(const PRUnichar * aPrinterName) { return _to SetPrinterName(aPrinterName); } \
  NS_IMETHOD GetPrintToFile(PRBool *aPrintToFile) { return _to GetPrintToFile(aPrintToFile); } \
  NS_IMETHOD SetPrintToFile(PRBool aPrintToFile) { return _to SetPrintToFile(aPrintToFile); } \
  NS_IMETHOD GetToFileName(PRUnichar * *aToFileName) { return _to GetToFileName(aToFileName); } \
  NS_IMETHOD SetToFileName(const PRUnichar * aToFileName) { return _to SetToFileName(aToFileName); } \
  NS_IMETHOD GetPrintPageDelay(PRInt32 *aPrintPageDelay) { return _to GetPrintPageDelay(aPrintPageDelay); } \
  NS_IMETHOD SetPrintPageDelay(PRInt32 aPrintPageDelay) { return _to SetPrintPageDelay(aPrintPageDelay); } \
  NS_IMETHOD GetIsInitializedFromPrinter(PRBool *aIsInitializedFromPrinter) { return _to GetIsInitializedFromPrinter(aIsInitializedFromPrinter); } \
  NS_IMETHOD SetIsInitializedFromPrinter(PRBool aIsInitializedFromPrinter) { return _to SetIsInitializedFromPrinter(aIsInitializedFromPrinter); } \
  NS_IMETHOD GetIsInitializedFromPrefs(PRBool *aIsInitializedFromPrefs) { return _to GetIsInitializedFromPrefs(aIsInitializedFromPrefs); } \
  NS_IMETHOD SetIsInitializedFromPrefs(PRBool aIsInitializedFromPrefs) { return _to SetIsInitializedFromPrefs(aIsInitializedFromPrefs); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPRINTSETTINGS(_to) \
  NS_IMETHOD SetPrintOptions(PRInt32 type, PRBool turnOnOff) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintOptions(type, turnOnOff); } \
  NS_IMETHOD GetPrintOptions(PRInt32 type, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintOptions(type, _retval); } \
  NS_IMETHOD GetPrintOptionsBits(PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintOptionsBits(_retval); } \
  NS_IMETHOD GetPageSizeInTwips(PRInt32 *width, PRInt32 *height) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPageSizeInTwips(width, height); } \
  NS_IMETHOD Clone(nsIPrintSettings **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Clone(_retval); } \
  NS_IMETHOD Assign(nsIPrintSettings *ps) { return !_to ? NS_ERROR_NULL_POINTER : _to->Assign(ps); } \
  NS_IMETHOD GetPrintSession(nsIPrintSession * *aPrintSession) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintSession(aPrintSession); } \
  NS_IMETHOD SetPrintSession(nsIPrintSession * aPrintSession) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintSession(aPrintSession); } \
  NS_IMETHOD GetStartPageRange(PRInt32 *aStartPageRange) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetStartPageRange(aStartPageRange); } \
  NS_IMETHOD SetStartPageRange(PRInt32 aStartPageRange) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetStartPageRange(aStartPageRange); } \
  NS_IMETHOD GetEndPageRange(PRInt32 *aEndPageRange) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetEndPageRange(aEndPageRange); } \
  NS_IMETHOD SetEndPageRange(PRInt32 aEndPageRange) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetEndPageRange(aEndPageRange); } \
  NS_IMETHOD GetMarginTop(double *aMarginTop) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMarginTop(aMarginTop); } \
  NS_IMETHOD SetMarginTop(double aMarginTop) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMarginTop(aMarginTop); } \
  NS_IMETHOD GetMarginLeft(double *aMarginLeft) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMarginLeft(aMarginLeft); } \
  NS_IMETHOD SetMarginLeft(double aMarginLeft) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMarginLeft(aMarginLeft); } \
  NS_IMETHOD GetMarginBottom(double *aMarginBottom) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMarginBottom(aMarginBottom); } \
  NS_IMETHOD SetMarginBottom(double aMarginBottom) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMarginBottom(aMarginBottom); } \
  NS_IMETHOD GetMarginRight(double *aMarginRight) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMarginRight(aMarginRight); } \
  NS_IMETHOD SetMarginRight(double aMarginRight) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMarginRight(aMarginRight); } \
  NS_IMETHOD GetScaling(double *aScaling) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScaling(aScaling); } \
  NS_IMETHOD SetScaling(double aScaling) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetScaling(aScaling); } \
  NS_IMETHOD GetPrintBGColors(PRBool *aPrintBGColors) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintBGColors(aPrintBGColors); } \
  NS_IMETHOD SetPrintBGColors(PRBool aPrintBGColors) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintBGColors(aPrintBGColors); } \
  NS_IMETHOD GetPrintBGImages(PRBool *aPrintBGImages) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintBGImages(aPrintBGImages); } \
  NS_IMETHOD SetPrintBGImages(PRBool aPrintBGImages) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintBGImages(aPrintBGImages); } \
  NS_IMETHOD GetPrintRange(PRInt16 *aPrintRange) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintRange(aPrintRange); } \
  NS_IMETHOD SetPrintRange(PRInt16 aPrintRange) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintRange(aPrintRange); } \
  NS_IMETHOD GetTitle(PRUnichar * *aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTitle(aTitle); } \
  NS_IMETHOD SetTitle(const PRUnichar * aTitle) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetTitle(aTitle); } \
  NS_IMETHOD GetDocURL(PRUnichar * *aDocURL) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDocURL(aDocURL); } \
  NS_IMETHOD SetDocURL(const PRUnichar * aDocURL) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDocURL(aDocURL); } \
  NS_IMETHOD GetHeaderStrLeft(PRUnichar * *aHeaderStrLeft) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHeaderStrLeft(aHeaderStrLeft); } \
  NS_IMETHOD SetHeaderStrLeft(const PRUnichar * aHeaderStrLeft) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHeaderStrLeft(aHeaderStrLeft); } \
  NS_IMETHOD GetHeaderStrCenter(PRUnichar * *aHeaderStrCenter) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHeaderStrCenter(aHeaderStrCenter); } \
  NS_IMETHOD SetHeaderStrCenter(const PRUnichar * aHeaderStrCenter) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHeaderStrCenter(aHeaderStrCenter); } \
  NS_IMETHOD GetHeaderStrRight(PRUnichar * *aHeaderStrRight) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHeaderStrRight(aHeaderStrRight); } \
  NS_IMETHOD SetHeaderStrRight(const PRUnichar * aHeaderStrRight) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHeaderStrRight(aHeaderStrRight); } \
  NS_IMETHOD GetFooterStrLeft(PRUnichar * *aFooterStrLeft) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFooterStrLeft(aFooterStrLeft); } \
  NS_IMETHOD SetFooterStrLeft(const PRUnichar * aFooterStrLeft) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFooterStrLeft(aFooterStrLeft); } \
  NS_IMETHOD GetFooterStrCenter(PRUnichar * *aFooterStrCenter) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFooterStrCenter(aFooterStrCenter); } \
  NS_IMETHOD SetFooterStrCenter(const PRUnichar * aFooterStrCenter) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFooterStrCenter(aFooterStrCenter); } \
  NS_IMETHOD GetFooterStrRight(PRUnichar * *aFooterStrRight) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFooterStrRight(aFooterStrRight); } \
  NS_IMETHOD SetFooterStrRight(const PRUnichar * aFooterStrRight) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFooterStrRight(aFooterStrRight); } \
  NS_IMETHOD GetHowToEnableFrameUI(PRInt16 *aHowToEnableFrameUI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHowToEnableFrameUI(aHowToEnableFrameUI); } \
  NS_IMETHOD SetHowToEnableFrameUI(PRInt16 aHowToEnableFrameUI) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHowToEnableFrameUI(aHowToEnableFrameUI); } \
  NS_IMETHOD GetIsCancelled(PRBool *aIsCancelled) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsCancelled(aIsCancelled); } \
  NS_IMETHOD SetIsCancelled(PRBool aIsCancelled) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetIsCancelled(aIsCancelled); } \
  NS_IMETHOD GetPrintFrameTypeUsage(PRInt16 *aPrintFrameTypeUsage) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintFrameTypeUsage(aPrintFrameTypeUsage); } \
  NS_IMETHOD SetPrintFrameTypeUsage(PRInt16 aPrintFrameTypeUsage) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintFrameTypeUsage(aPrintFrameTypeUsage); } \
  NS_IMETHOD GetPrintFrameType(PRInt16 *aPrintFrameType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintFrameType(aPrintFrameType); } \
  NS_IMETHOD SetPrintFrameType(PRInt16 aPrintFrameType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintFrameType(aPrintFrameType); } \
  NS_IMETHOD GetPrintSilent(PRBool *aPrintSilent) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintSilent(aPrintSilent); } \
  NS_IMETHOD SetPrintSilent(PRBool aPrintSilent) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintSilent(aPrintSilent); } \
  NS_IMETHOD GetShrinkToFit(PRBool *aShrinkToFit) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetShrinkToFit(aShrinkToFit); } \
  NS_IMETHOD SetShrinkToFit(PRBool aShrinkToFit) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetShrinkToFit(aShrinkToFit); } \
  NS_IMETHOD GetShowPrintProgress(PRBool *aShowPrintProgress) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetShowPrintProgress(aShowPrintProgress); } \
  NS_IMETHOD SetShowPrintProgress(PRBool aShowPrintProgress) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetShowPrintProgress(aShowPrintProgress); } \
  NS_IMETHOD GetPaperName(PRUnichar * *aPaperName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPaperName(aPaperName); } \
  NS_IMETHOD SetPaperName(const PRUnichar * aPaperName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPaperName(aPaperName); } \
  NS_IMETHOD GetPaperSizeType(PRInt16 *aPaperSizeType) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPaperSizeType(aPaperSizeType); } \
  NS_IMETHOD SetPaperSizeType(PRInt16 aPaperSizeType) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPaperSizeType(aPaperSizeType); } \
  NS_IMETHOD GetPaperData(PRInt16 *aPaperData) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPaperData(aPaperData); } \
  NS_IMETHOD SetPaperData(PRInt16 aPaperData) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPaperData(aPaperData); } \
  NS_IMETHOD GetPaperWidth(double *aPaperWidth) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPaperWidth(aPaperWidth); } \
  NS_IMETHOD SetPaperWidth(double aPaperWidth) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPaperWidth(aPaperWidth); } \
  NS_IMETHOD GetPaperHeight(double *aPaperHeight) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPaperHeight(aPaperHeight); } \
  NS_IMETHOD SetPaperHeight(double aPaperHeight) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPaperHeight(aPaperHeight); } \
  NS_IMETHOD GetPaperSizeUnit(PRInt16 *aPaperSizeUnit) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPaperSizeUnit(aPaperSizeUnit); } \
  NS_IMETHOD SetPaperSizeUnit(PRInt16 aPaperSizeUnit) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPaperSizeUnit(aPaperSizeUnit); } \
  NS_IMETHOD GetPlexName(PRUnichar * *aPlexName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPlexName(aPlexName); } \
  NS_IMETHOD SetPlexName(const PRUnichar * aPlexName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPlexName(aPlexName); } \
  NS_IMETHOD GetColorspace(PRUnichar * *aColorspace) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetColorspace(aColorspace); } \
  NS_IMETHOD SetColorspace(const PRUnichar * aColorspace) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetColorspace(aColorspace); } \
  NS_IMETHOD GetResolutionName(PRUnichar * *aResolutionName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetResolutionName(aResolutionName); } \
  NS_IMETHOD SetResolutionName(const PRUnichar * aResolutionName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetResolutionName(aResolutionName); } \
  NS_IMETHOD GetDownloadFonts(PRBool *aDownloadFonts) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDownloadFonts(aDownloadFonts); } \
  NS_IMETHOD SetDownloadFonts(PRBool aDownloadFonts) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetDownloadFonts(aDownloadFonts); } \
  NS_IMETHOD GetPrintReversed(PRBool *aPrintReversed) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintReversed(aPrintReversed); } \
  NS_IMETHOD SetPrintReversed(PRBool aPrintReversed) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintReversed(aPrintReversed); } \
  NS_IMETHOD GetPrintInColor(PRBool *aPrintInColor) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintInColor(aPrintInColor); } \
  NS_IMETHOD SetPrintInColor(PRBool aPrintInColor) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintInColor(aPrintInColor); } \
  NS_IMETHOD GetPaperSize(PRInt32 *aPaperSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPaperSize(aPaperSize); } \
  NS_IMETHOD SetPaperSize(PRInt32 aPaperSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPaperSize(aPaperSize); } \
  NS_IMETHOD GetOrientation(PRInt32 *aOrientation) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOrientation(aOrientation); } \
  NS_IMETHOD SetOrientation(PRInt32 aOrientation) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetOrientation(aOrientation); } \
  NS_IMETHOD GetPrintCommand(PRUnichar * *aPrintCommand) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintCommand(aPrintCommand); } \
  NS_IMETHOD SetPrintCommand(const PRUnichar * aPrintCommand) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintCommand(aPrintCommand); } \
  NS_IMETHOD GetNumCopies(PRInt32 *aNumCopies) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNumCopies(aNumCopies); } \
  NS_IMETHOD SetNumCopies(PRInt32 aNumCopies) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetNumCopies(aNumCopies); } \
  NS_IMETHOD GetPrinterName(PRUnichar * *aPrinterName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrinterName(aPrinterName); } \
  NS_IMETHOD SetPrinterName(const PRUnichar * aPrinterName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrinterName(aPrinterName); } \
  NS_IMETHOD GetPrintToFile(PRBool *aPrintToFile) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintToFile(aPrintToFile); } \
  NS_IMETHOD SetPrintToFile(PRBool aPrintToFile) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintToFile(aPrintToFile); } \
  NS_IMETHOD GetToFileName(PRUnichar * *aToFileName) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetToFileName(aToFileName); } \
  NS_IMETHOD SetToFileName(const PRUnichar * aToFileName) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetToFileName(aToFileName); } \
  NS_IMETHOD GetPrintPageDelay(PRInt32 *aPrintPageDelay) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrintPageDelay(aPrintPageDelay); } \
  NS_IMETHOD SetPrintPageDelay(PRInt32 aPrintPageDelay) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPrintPageDelay(aPrintPageDelay); } \
  NS_IMETHOD GetIsInitializedFromPrinter(PRBool *aIsInitializedFromPrinter) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsInitializedFromPrinter(aIsInitializedFromPrinter); } \
  NS_IMETHOD SetIsInitializedFromPrinter(PRBool aIsInitializedFromPrinter) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetIsInitializedFromPrinter(aIsInitializedFromPrinter); } \
  NS_IMETHOD GetIsInitializedFromPrefs(PRBool *aIsInitializedFromPrefs) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsInitializedFromPrefs(aIsInitializedFromPrefs); } \
  NS_IMETHOD SetIsInitializedFromPrefs(PRBool aIsInitializedFromPrefs) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetIsInitializedFromPrefs(aIsInitializedFromPrefs); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsPrintSettings : public nsIPrintSettings
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPRINTSETTINGS

  nsPrintSettings();

private:
  ~nsPrintSettings();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsPrintSettings, nsIPrintSettings)

nsPrintSettings::nsPrintSettings()
{
  /* member initializers and constructor code */
}

nsPrintSettings::~nsPrintSettings()
{
  /* destructor code */
}

/* void SetPrintOptions (in long type, in boolean turnOnOff); */
NS_IMETHODIMP nsPrintSettings::SetPrintOptions(PRInt32 type, PRBool turnOnOff)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean GetPrintOptions (in long type); */
NS_IMETHODIMP nsPrintSettings::GetPrintOptions(PRInt32 type, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long GetPrintOptionsBits (); */
NS_IMETHODIMP nsPrintSettings::GetPrintOptionsBits(PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void GetPageSizeInTwips (out long width, out long height); */
NS_IMETHODIMP nsPrintSettings::GetPageSizeInTwips(PRInt32 *width, PRInt32 *height)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrintSettings clone (); */
NS_IMETHODIMP nsPrintSettings::Clone(nsIPrintSettings **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void assign (in nsIPrintSettings ps); */
NS_IMETHODIMP nsPrintSettings::Assign(nsIPrintSettings *ps)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] attribute nsIPrintSession printSession; */
NS_IMETHODIMP nsPrintSettings::GetPrintSession(nsIPrintSession * *aPrintSession)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintSession(nsIPrintSession * aPrintSession)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long startPageRange; */
NS_IMETHODIMP nsPrintSettings::GetStartPageRange(PRInt32 *aStartPageRange)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetStartPageRange(PRInt32 aStartPageRange)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long endPageRange; */
NS_IMETHODIMP nsPrintSettings::GetEndPageRange(PRInt32 *aEndPageRange)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetEndPageRange(PRInt32 aEndPageRange)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute double marginTop; */
NS_IMETHODIMP nsPrintSettings::GetMarginTop(double *aMarginTop)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetMarginTop(double aMarginTop)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute double marginLeft; */
NS_IMETHODIMP nsPrintSettings::GetMarginLeft(double *aMarginLeft)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetMarginLeft(double aMarginLeft)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute double marginBottom; */
NS_IMETHODIMP nsPrintSettings::GetMarginBottom(double *aMarginBottom)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetMarginBottom(double aMarginBottom)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute double marginRight; */
NS_IMETHODIMP nsPrintSettings::GetMarginRight(double *aMarginRight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetMarginRight(double aMarginRight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute double scaling; */
NS_IMETHODIMP nsPrintSettings::GetScaling(double *aScaling)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetScaling(double aScaling)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean printBGColors; */
NS_IMETHODIMP nsPrintSettings::GetPrintBGColors(PRBool *aPrintBGColors)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintBGColors(PRBool aPrintBGColors)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean printBGImages; */
NS_IMETHODIMP nsPrintSettings::GetPrintBGImages(PRBool *aPrintBGImages)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintBGImages(PRBool aPrintBGImages)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute short printRange; */
NS_IMETHODIMP nsPrintSettings::GetPrintRange(PRInt16 *aPrintRange)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintRange(PRInt16 aPrintRange)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring title; */
NS_IMETHODIMP nsPrintSettings::GetTitle(PRUnichar * *aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetTitle(const PRUnichar * aTitle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring docURL; */
NS_IMETHODIMP nsPrintSettings::GetDocURL(PRUnichar * *aDocURL)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetDocURL(const PRUnichar * aDocURL)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring headerStrLeft; */
NS_IMETHODIMP nsPrintSettings::GetHeaderStrLeft(PRUnichar * *aHeaderStrLeft)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetHeaderStrLeft(const PRUnichar * aHeaderStrLeft)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring headerStrCenter; */
NS_IMETHODIMP nsPrintSettings::GetHeaderStrCenter(PRUnichar * *aHeaderStrCenter)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetHeaderStrCenter(const PRUnichar * aHeaderStrCenter)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring headerStrRight; */
NS_IMETHODIMP nsPrintSettings::GetHeaderStrRight(PRUnichar * *aHeaderStrRight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetHeaderStrRight(const PRUnichar * aHeaderStrRight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring footerStrLeft; */
NS_IMETHODIMP nsPrintSettings::GetFooterStrLeft(PRUnichar * *aFooterStrLeft)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetFooterStrLeft(const PRUnichar * aFooterStrLeft)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring footerStrCenter; */
NS_IMETHODIMP nsPrintSettings::GetFooterStrCenter(PRUnichar * *aFooterStrCenter)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetFooterStrCenter(const PRUnichar * aFooterStrCenter)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring footerStrRight; */
NS_IMETHODIMP nsPrintSettings::GetFooterStrRight(PRUnichar * *aFooterStrRight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetFooterStrRight(const PRUnichar * aFooterStrRight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute short howToEnableFrameUI; */
NS_IMETHODIMP nsPrintSettings::GetHowToEnableFrameUI(PRInt16 *aHowToEnableFrameUI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetHowToEnableFrameUI(PRInt16 aHowToEnableFrameUI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean isCancelled; */
NS_IMETHODIMP nsPrintSettings::GetIsCancelled(PRBool *aIsCancelled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetIsCancelled(PRBool aIsCancelled)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute short printFrameTypeUsage; */
NS_IMETHODIMP nsPrintSettings::GetPrintFrameTypeUsage(PRInt16 *aPrintFrameTypeUsage)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintFrameTypeUsage(PRInt16 aPrintFrameTypeUsage)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute short printFrameType; */
NS_IMETHODIMP nsPrintSettings::GetPrintFrameType(PRInt16 *aPrintFrameType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintFrameType(PRInt16 aPrintFrameType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean printSilent; */
NS_IMETHODIMP nsPrintSettings::GetPrintSilent(PRBool *aPrintSilent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintSilent(PRBool aPrintSilent)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean shrinkToFit; */
NS_IMETHODIMP nsPrintSettings::GetShrinkToFit(PRBool *aShrinkToFit)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetShrinkToFit(PRBool aShrinkToFit)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean showPrintProgress; */
NS_IMETHODIMP nsPrintSettings::GetShowPrintProgress(PRBool *aShowPrintProgress)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetShowPrintProgress(PRBool aShowPrintProgress)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring paperName; */
NS_IMETHODIMP nsPrintSettings::GetPaperName(PRUnichar * *aPaperName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPaperName(const PRUnichar * aPaperName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute short paperSizeType; */
NS_IMETHODIMP nsPrintSettings::GetPaperSizeType(PRInt16 *aPaperSizeType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPaperSizeType(PRInt16 aPaperSizeType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute short paperData; */
NS_IMETHODIMP nsPrintSettings::GetPaperData(PRInt16 *aPaperData)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPaperData(PRInt16 aPaperData)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute double paperWidth; */
NS_IMETHODIMP nsPrintSettings::GetPaperWidth(double *aPaperWidth)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPaperWidth(double aPaperWidth)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute double paperHeight; */
NS_IMETHODIMP nsPrintSettings::GetPaperHeight(double *aPaperHeight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPaperHeight(double aPaperHeight)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute short paperSizeUnit; */
NS_IMETHODIMP nsPrintSettings::GetPaperSizeUnit(PRInt16 *aPaperSizeUnit)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPaperSizeUnit(PRInt16 aPaperSizeUnit)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring plexName; */
NS_IMETHODIMP nsPrintSettings::GetPlexName(PRUnichar * *aPlexName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPlexName(const PRUnichar * aPlexName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring colorspace; */
NS_IMETHODIMP nsPrintSettings::GetColorspace(PRUnichar * *aColorspace)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetColorspace(const PRUnichar * aColorspace)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring resolutionName; */
NS_IMETHODIMP nsPrintSettings::GetResolutionName(PRUnichar * *aResolutionName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetResolutionName(const PRUnichar * aResolutionName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean downloadFonts; */
NS_IMETHODIMP nsPrintSettings::GetDownloadFonts(PRBool *aDownloadFonts)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetDownloadFonts(PRBool aDownloadFonts)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean printReversed; */
NS_IMETHODIMP nsPrintSettings::GetPrintReversed(PRBool *aPrintReversed)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintReversed(PRBool aPrintReversed)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean printInColor; */
NS_IMETHODIMP nsPrintSettings::GetPrintInColor(PRBool *aPrintInColor)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintInColor(PRBool aPrintInColor)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long paperSize; */
NS_IMETHODIMP nsPrintSettings::GetPaperSize(PRInt32 *aPaperSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPaperSize(PRInt32 aPaperSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long orientation; */
NS_IMETHODIMP nsPrintSettings::GetOrientation(PRInt32 *aOrientation)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetOrientation(PRInt32 aOrientation)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring printCommand; */
NS_IMETHODIMP nsPrintSettings::GetPrintCommand(PRUnichar * *aPrintCommand)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintCommand(const PRUnichar * aPrintCommand)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long numCopies; */
NS_IMETHODIMP nsPrintSettings::GetNumCopies(PRInt32 *aNumCopies)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetNumCopies(PRInt32 aNumCopies)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring printerName; */
NS_IMETHODIMP nsPrintSettings::GetPrinterName(PRUnichar * *aPrinterName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrinterName(const PRUnichar * aPrinterName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean printToFile; */
NS_IMETHODIMP nsPrintSettings::GetPrintToFile(PRBool *aPrintToFile)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintToFile(PRBool aPrintToFile)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring toFileName; */
NS_IMETHODIMP nsPrintSettings::GetToFileName(PRUnichar * *aToFileName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetToFileName(const PRUnichar * aToFileName)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long printPageDelay; */
NS_IMETHODIMP nsPrintSettings::GetPrintPageDelay(PRInt32 *aPrintPageDelay)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetPrintPageDelay(PRInt32 aPrintPageDelay)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean isInitializedFromPrinter; */
NS_IMETHODIMP nsPrintSettings::GetIsInitializedFromPrinter(PRBool *aIsInitializedFromPrinter)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetIsInitializedFromPrinter(PRBool aIsInitializedFromPrinter)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean isInitializedFromPrefs; */
NS_IMETHODIMP nsPrintSettings::GetIsInitializedFromPrefs(PRBool *aIsInitializedFromPrefs)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsPrintSettings::SetIsInitializedFromPrefs(PRBool aIsInitializedFromPrefs)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMWindow; /* forward declaration */


/* starting interface:    nsIPromptService */
#define NS_IPROMPTSERVICE_IID_STR "1630c61a-325e-49ca-8759-a31b16c47aa5"

#define NS_IPROMPTSERVICE_IID \
  {0x1630c61a, 0x325e, 0x49ca, \
    { 0x87, 0x59, 0xa3, 0x1b, 0x16, 0xc4, 0x7a, 0xa5 }}

class NS_NO_VTABLE nsIPromptService : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IPROMPTSERVICE_IID)

  /* void alert (in nsIDOMWindow parent, in wstring dialog_title, in wstring text); */
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text) = 0;

  /* void alertCheck (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in wstring check_msg, inout boolean check_state); */
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state) = 0;

  /* boolean confirm (in nsIDOMWindow parent, in wstring dialog_title, in wstring text); */
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRBool *_retval) = 0;

  /* boolean confirmCheck (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in wstring check_msg, inout boolean check_state); */
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) = 0;

  enum { BUTTON_POS_0 = 1U };

  enum { BUTTON_POS_1 = 256U };

  enum { BUTTON_POS_2 = 65536U };

  enum { BUTTON_TITLE_OK = 1U };

  enum { BUTTON_TITLE_CANCEL = 2U };

  enum { BUTTON_TITLE_YES = 3U };

  enum { BUTTON_TITLE_NO = 4U };

  enum { BUTTON_TITLE_SAVE = 5U };

  enum { BUTTON_TITLE_DONT_SAVE = 6U };

  enum { BUTTON_TITLE_REVERT = 7U };

  enum { BUTTON_TITLE_IS_STRING = 127U };

  enum { BUTTON_POS_0_DEFAULT = 0U };

  enum { BUTTON_POS_1_DEFAULT = 16777216U };

  enum { BUTTON_POS_2_DEFAULT = 33554432U };

  enum { BUTTON_DELAY_ENABLE = 67108864U };

  enum { STD_OK_CANCEL_BUTTONS = 513U };

  enum { STD_YES_NO_BUTTONS = 1027U };

  /* PRInt32 confirmEx (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in unsigned long button_flags, in wstring button0_title, in wstring button1_title, in wstring button2_title, in wstring check_msg, inout boolean check_state); */
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 button_flags, const PRUnichar *button0_title, const PRUnichar *button1_title, const PRUnichar *button2_title, const PRUnichar *check_msg, PRBool *check_state, PRInt32 *_retval) = 0;

  /* boolean prompt (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, inout wstring value, in wstring check_msg, inout boolean check_state); */
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **value, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) = 0;

  /* boolean promptUsernameAndPassword (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, inout wstring username, inout wstring password, in wstring check_msg, inout boolean check_state); */
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) = 0;

  /* boolean promptPassword (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, inout wstring password, in wstring check_msg, inout boolean check_state); */
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) = 0;

  /* boolean select (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in PRUint32 count, [array, size_is (count)] in wstring select_list, out long out_selection); */
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 count, const PRUnichar **select_list, PRInt32 *out_selection, PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPROMPTSERVICE \
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text); \
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state); \
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRBool *_retval); \
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval); \
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 button_flags, const PRUnichar *button0_title, const PRUnichar *button1_title, const PRUnichar *button2_title, const PRUnichar *check_msg, PRBool *check_state, PRInt32 *_retval); \
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **value, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval); \
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval); \
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval); \
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 count, const PRUnichar **select_list, PRInt32 *out_selection, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPROMPTSERVICE(_to) \
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text) { return _to Alert(parent, dialog_title, text); } \
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state) { return _to AlertCheck(parent, dialog_title, text, check_msg, check_state); } \
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRBool *_retval) { return _to Confirm(parent, dialog_title, text, _retval); } \
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return _to ConfirmCheck(parent, dialog_title, text, check_msg, check_state, _retval); } \
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 button_flags, const PRUnichar *button0_title, const PRUnichar *button1_title, const PRUnichar *button2_title, const PRUnichar *check_msg, PRBool *check_state, PRInt32 *_retval) { return _to ConfirmEx(parent, dialog_title, text, button_flags, button0_title, button1_title, button2_title, check_msg, check_state, _retval); } \
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **value, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return _to Prompt(parent, dialog_title, text, value, check_msg, check_state, _retval); } \
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return _to PromptUsernameAndPassword(parent, dialog_title, text, username, password, check_msg, check_state, _retval); } \
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return _to PromptPassword(parent, dialog_title, text, password, check_msg, check_state, _retval); } \
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 count, const PRUnichar **select_list, PRInt32 *out_selection, PRBool *_retval) { return _to Select(parent, dialog_title, text, count, select_list, out_selection, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPROMPTSERVICE(_to) \
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text) { return !_to ? NS_ERROR_NULL_POINTER : _to->Alert(parent, dialog_title, text); } \
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state) { return !_to ? NS_ERROR_NULL_POINTER : _to->AlertCheck(parent, dialog_title, text, check_msg, check_state); } \
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Confirm(parent, dialog_title, text, _retval); } \
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConfirmCheck(parent, dialog_title, text, check_msg, check_state, _retval); } \
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 button_flags, const PRUnichar *button0_title, const PRUnichar *button1_title, const PRUnichar *button2_title, const PRUnichar *check_msg, PRBool *check_state, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConfirmEx(parent, dialog_title, text, button_flags, button0_title, button1_title, button2_title, check_msg, check_state, _retval); } \
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **value, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Prompt(parent, dialog_title, text, value, check_msg, check_state, _retval); } \
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->PromptUsernameAndPassword(parent, dialog_title, text, username, password, check_msg, check_state, _retval); } \
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->PromptPassword(parent, dialog_title, text, password, check_msg, check_state, _retval); } \
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 count, const PRUnichar **select_list, PRInt32 *out_selection, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Select(parent, dialog_title, text, count, select_list, out_selection, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsPromptService : public nsIPromptService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPROMPTSERVICE

  nsPromptService();

private:
  ~nsPromptService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsPromptService, nsIPromptService)

nsPromptService::nsPromptService()
{
  /* member initializers and constructor code */
}

nsPromptService::~nsPromptService()
{
  /* destructor code */
}

/* void alert (in nsIDOMWindow parent, in wstring dialog_title, in wstring text); */
NS_IMETHODIMP nsPromptService::Alert(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void alertCheck (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in wstring check_msg, inout boolean check_state); */
NS_IMETHODIMP nsPromptService::AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean confirm (in nsIDOMWindow parent, in wstring dialog_title, in wstring text); */
NS_IMETHODIMP nsPromptService::Confirm(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean confirmCheck (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in wstring check_msg, inout boolean check_state); */
NS_IMETHODIMP nsPromptService::ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRInt32 confirmEx (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in unsigned long button_flags, in wstring button0_title, in wstring button1_title, in wstring button2_title, in wstring check_msg, inout boolean check_state); */
NS_IMETHODIMP nsPromptService::ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 button_flags, const PRUnichar *button0_title, const PRUnichar *button1_title, const PRUnichar *button2_title, const PRUnichar *check_msg, PRBool *check_state, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean prompt (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, inout wstring value, in wstring check_msg, inout boolean check_state); */
NS_IMETHODIMP nsPromptService::Prompt(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **value, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean promptUsernameAndPassword (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, inout wstring username, inout wstring password, in wstring check_msg, inout boolean check_state); */
NS_IMETHODIMP nsPromptService::PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean promptPassword (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, inout wstring password, in wstring check_msg, inout boolean check_state); */
NS_IMETHODIMP nsPromptService::PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUnichar **password, const PRUnichar *check_msg, PRBool *check_state, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean select (in nsIDOMWindow parent, in wstring dialog_title, in wstring text, in PRUint32 count, [array, size_is (count)] in wstring select_list, out long out_selection); */
NS_IMETHODIMP nsPromptService::Select(nsIDOMWindow *parent, const PRUnichar *dialog_title, const PRUnichar *text, PRUint32 count, const PRUnichar **select_list, PRInt32 *out_selection, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIProperties */
#define NS_IPROPERTIES_IID_STR "78650582-4e93-4b60-8e85-26ebd3eb14ca"

#define NS_IPROPERTIES_IID \
  {0x78650582, 0x4e93, 0x4b60, \
    { 0x8e, 0x85, 0x26, 0xeb, 0xd3, 0xeb, 0x14, 0xca }}

class NS_NO_VTABLE nsIProperties : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IPROPERTIES_IID)

  /* void get (in string prop_name, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD Get(const char *prop_name, const nsIID & iid, void * *result) = 0;

  /* void set (in string prop_name, in nsISupports value); */
  NS_IMETHOD Set(const char *prop_name, nsISupports *value) = 0;

  /* boolean has (in string prop_name); */
  NS_IMETHOD Has(const char *prop_name, PRBool *_retval) = 0;

  /* void undefine (in string prop_name); */
  NS_IMETHOD Undefine(const char *prop_name) = 0;

  /* void getKeys (out PRUint32 count, [array, size_is (count), retval] out string keys); */
  NS_IMETHOD GetKeys(PRUint32 *count, char ***keys) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPROPERTIES \
  NS_IMETHOD Get(const char *prop_name, const nsIID & iid, void * *result); \
  NS_IMETHOD Set(const char *prop_name, nsISupports *value); \
  NS_IMETHOD Has(const char *prop_name, PRBool *_retval); \
  NS_IMETHOD Undefine(const char *prop_name); \
  NS_IMETHOD GetKeys(PRUint32 *count, char ***keys); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPROPERTIES(_to) \
  NS_IMETHOD Get(const char *prop_name, const nsIID & iid, void * *result) { return _to Get(prop_name, iid, result); } \
  NS_IMETHOD Set(const char *prop_name, nsISupports *value) { return _to Set(prop_name, value); } \
  NS_IMETHOD Has(const char *prop_name, PRBool *_retval) { return _to Has(prop_name, _retval); } \
  NS_IMETHOD Undefine(const char *prop_name) { return _to Undefine(prop_name); } \
  NS_IMETHOD GetKeys(PRUint32 *count, char ***keys) { return _to GetKeys(count, keys); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPROPERTIES(_to) \
  NS_IMETHOD Get(const char *prop_name, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->Get(prop_name, iid, result); } \
  NS_IMETHOD Set(const char *prop_name, nsISupports *value) { return !_to ? NS_ERROR_NULL_POINTER : _to->Set(prop_name, value); } \
  NS_IMETHOD Has(const char *prop_name, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Has(prop_name, _retval); } \
  NS_IMETHOD Undefine(const char *prop_name) { return !_to ? NS_ERROR_NULL_POINTER : _to->Undefine(prop_name); } \
  NS_IMETHOD GetKeys(PRUint32 *count, char ***keys) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetKeys(count, keys); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsProperties : public nsIProperties
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPROPERTIES

  nsProperties();

private:
  ~nsProperties();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsProperties, nsIProperties)

nsProperties::nsProperties()
{
  /* member initializers and constructor code */
}

nsProperties::~nsProperties()
{
  /* destructor code */
}

/* void get (in string prop_name, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsProperties::Get(const char *prop_name, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void set (in string prop_name, in nsISupports value); */
NS_IMETHODIMP nsProperties::Set(const char *prop_name, nsISupports *value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean has (in string prop_name); */
NS_IMETHODIMP nsProperties::Has(const char *prop_name, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void undefine (in string prop_name); */
NS_IMETHODIMP nsProperties::Undefine(const char *prop_name)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getKeys (out PRUint32 count, [array, size_is (count), retval] out string keys); */
NS_IMETHODIMP nsProperties::GetKeys(PRUint32 *count, char ***keys)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIRequest; /* forward declaration */


/* starting interface:    nsIRequestObserver */
#define NS_IREQUESTOBSERVER_IID_STR "fd91e2e0-1481-11d3-9333-00104ba0fd40"

#define NS_IREQUESTOBSERVER_IID \
  {0xfd91e2e0, 0x1481, 0x11d3, \
    { 0x93, 0x33, 0x00, 0x10, 0x4b, 0xa0, 0xfd, 0x40 }}

class NS_NO_VTABLE nsIRequestObserver : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IREQUESTOBSERVER_IID)

  /* void onStartRequest (in nsIRequest request, in nsISupports contxt); */
  NS_IMETHOD OnStartRequest(nsIRequest *request, nsISupports *contxt) = 0;

  /* void onStopRequest (in nsIRequest request, in nsISupports contxt, in nsresult status_code); */
  NS_IMETHOD OnStopRequest(nsIRequest *request, nsISupports *contxt, nsresult status_code) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIREQUESTOBSERVER \
  NS_IMETHOD OnStartRequest(nsIRequest *request, nsISupports *contxt); \
  NS_IMETHOD OnStopRequest(nsIRequest *request, nsISupports *contxt, nsresult status_code); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIREQUESTOBSERVER(_to) \
  NS_IMETHOD OnStartRequest(nsIRequest *request, nsISupports *contxt) { return _to OnStartRequest(request, contxt); } \
  NS_IMETHOD OnStopRequest(nsIRequest *request, nsISupports *contxt, nsresult status_code) { return _to OnStopRequest(request, contxt, status_code); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIREQUESTOBSERVER(_to) \
  NS_IMETHOD OnStartRequest(nsIRequest *request, nsISupports *contxt) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnStartRequest(request, contxt); } \
  NS_IMETHOD OnStopRequest(nsIRequest *request, nsISupports *contxt, nsresult status_code) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnStopRequest(request, contxt, status_code); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsRequestObserver : public nsIRequestObserver
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIREQUESTOBSERVER

  nsRequestObserver();

private:
  ~nsRequestObserver();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsRequestObserver, nsIRequestObserver)

nsRequestObserver::nsRequestObserver()
{
  /* member initializers and constructor code */
}

nsRequestObserver::~nsRequestObserver()
{
  /* destructor code */
}

/* void onStartRequest (in nsIRequest request, in nsISupports contxt); */
NS_IMETHODIMP nsRequestObserver::OnStartRequest(nsIRequest *request, nsISupports *contxt)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onStopRequest (in nsIRequest request, in nsISupports contxt, in nsresult status_code); */
NS_IMETHODIMP nsRequestObserver::OnStopRequest(nsIRequest *request, nsISupports *contxt, nsresult status_code)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIServiceManager */
#define NS_ISERVICEMANAGER_IID_STR "8bb35ed9-e332-462d-9155-4a002ab5c958"

#define NS_ISERVICEMANAGER_IID \
  {0x8bb35ed9, 0xe332, 0x462d, \
    { 0x91, 0x55, 0x4a, 0x00, 0x2a, 0xb5, 0xc9, 0x58 }}

class NS_NO_VTABLE nsIServiceManager : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISERVICEMANAGER_IID)

  /* void getService (in nsCIDRef class_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD GetService(const nsCID & class_id, const nsIID & iid, void * *result) = 0;

  /* void getServiceByContractID (in string contract_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
  NS_IMETHOD GetServiceByContractID(const char *contract_id, const nsIID & iid, void * *result) = 0;

  /* boolean isServiceInstantiated (in nsCIDRef class_id, in nsIIDRef iid); */
  NS_IMETHOD IsServiceInstantiated(const nsCID & class_id, const nsIID & iid, PRBool *_retval) = 0;

  /* boolean isServiceInstantiatedByContractID (in string contract_id, in nsIIDRef iid); */
  NS_IMETHOD IsServiceInstantiatedByContractID(const char *contract_id, const nsIID & iid, PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISERVICEMANAGER \
  NS_IMETHOD GetService(const nsCID & class_id, const nsIID & iid, void * *result); \
  NS_IMETHOD GetServiceByContractID(const char *contract_id, const nsIID & iid, void * *result); \
  NS_IMETHOD IsServiceInstantiated(const nsCID & class_id, const nsIID & iid, PRBool *_retval); \
  NS_IMETHOD IsServiceInstantiatedByContractID(const char *contract_id, const nsIID & iid, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISERVICEMANAGER(_to) \
  NS_IMETHOD GetService(const nsCID & class_id, const nsIID & iid, void * *result) { return _to GetService(class_id, iid, result); } \
  NS_IMETHOD GetServiceByContractID(const char *contract_id, const nsIID & iid, void * *result) { return _to GetServiceByContractID(contract_id, iid, result); } \
  NS_IMETHOD IsServiceInstantiated(const nsCID & class_id, const nsIID & iid, PRBool *_retval) { return _to IsServiceInstantiated(class_id, iid, _retval); } \
  NS_IMETHOD IsServiceInstantiatedByContractID(const char *contract_id, const nsIID & iid, PRBool *_retval) { return _to IsServiceInstantiatedByContractID(contract_id, iid, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISERVICEMANAGER(_to) \
  NS_IMETHOD GetService(const nsCID & class_id, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetService(class_id, iid, result); } \
  NS_IMETHOD GetServiceByContractID(const char *contract_id, const nsIID & iid, void * *result) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetServiceByContractID(contract_id, iid, result); } \
  NS_IMETHOD IsServiceInstantiated(const nsCID & class_id, const nsIID & iid, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsServiceInstantiated(class_id, iid, _retval); } \
  NS_IMETHOD IsServiceInstantiatedByContractID(const char *contract_id, const nsIID & iid, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsServiceInstantiatedByContractID(contract_id, iid, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsServiceManager : public nsIServiceManager
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISERVICEMANAGER

  nsServiceManager();

private:
  ~nsServiceManager();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsServiceManager, nsIServiceManager)

nsServiceManager::nsServiceManager()
{
  /* member initializers and constructor code */
}

nsServiceManager::~nsServiceManager()
{
  /* destructor code */
}

/* void getService (in nsCIDRef class_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsServiceManager::GetService(const nsCID & class_id, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getServiceByContractID (in string contract_id, in nsIIDRef iid, [iid_is (iid), retval] out nsQIResult result); */
NS_IMETHODIMP nsServiceManager::GetServiceByContractID(const char *contract_id, const nsIID & iid, void * *result)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isServiceInstantiated (in nsCIDRef class_id, in nsIIDRef iid); */
NS_IMETHODIMP nsServiceManager::IsServiceInstantiated(const nsCID & class_id, const nsIID & iid, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isServiceInstantiatedByContractID (in string contract_id, in nsIIDRef iid); */
NS_IMETHODIMP nsServiceManager::IsServiceInstantiatedByContractID(const char *contract_id, const nsIID & iid, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIRequest; /* forward declaration */

class nsIInputStream; /* forward declaration */


/* starting interface:    nsIStreamListener */
#define NS_ISTREAMLISTENER_IID_STR "1a637020-1482-11d3-9333-00104ba0fd40"

#define NS_ISTREAMLISTENER_IID \
  {0x1a637020, 0x1482, 0x11d3, \
    { 0x93, 0x33, 0x00, 0x10, 0x4b, 0xa0, 0xfd, 0x40 }}

class NS_NO_VTABLE nsIStreamListener : public nsIRequestObserver {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISTREAMLISTENER_IID)

  /* void onDataAvailable (in nsIRequest request, in nsISupports contxt, in nsIInputStream input_stream, in unsigned long offset, in unsigned long count); */
  NS_IMETHOD OnDataAvailable(nsIRequest *request, nsISupports *contxt, nsIInputStream *input_stream, PRUint32 offset, PRUint32 count) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISTREAMLISTENER \
  NS_IMETHOD OnDataAvailable(nsIRequest *request, nsISupports *contxt, nsIInputStream *input_stream, PRUint32 offset, PRUint32 count); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISTREAMLISTENER(_to) \
  NS_IMETHOD OnDataAvailable(nsIRequest *request, nsISupports *contxt, nsIInputStream *input_stream, PRUint32 offset, PRUint32 count) { return _to OnDataAvailable(request, contxt, input_stream, offset, count); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISTREAMLISTENER(_to) \
  NS_IMETHOD OnDataAvailable(nsIRequest *request, nsISupports *contxt, nsIInputStream *input_stream, PRUint32 offset, PRUint32 count) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnDataAvailable(request, contxt, input_stream, offset, count); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsStreamListener : public nsIStreamListener
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISTREAMLISTENER

  nsStreamListener();

private:
  ~nsStreamListener();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsStreamListener, nsIStreamListener)

nsStreamListener::nsStreamListener()
{
  /* member initializers and constructor code */
}

nsStreamListener::~nsStreamListener()
{
  /* destructor code */
}

/* void onDataAvailable (in nsIRequest request, in nsISupports contxt, in nsIInputStream input_stream, in unsigned long offset, in unsigned long count); */
NS_IMETHODIMP nsStreamListener::OnDataAvailable(nsIRequest *request, nsISupports *contxt, nsIInputStream *input_stream, PRUint32 offset, PRUint32 count)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIStringInputStream */
#define NS_ISTRINGINPUTSTREAM_IID_STR "450cd2d4-f0fd-424d-b365-b1251f80fd53"

#define NS_ISTRINGINPUTSTREAM_IID \
  {0x450cd2d4, 0xf0fd, 0x424d, \
    { 0xb3, 0x65, 0xb1, 0x25, 0x1f, 0x80, 0xfd, 0x53 }}

class NS_NO_VTABLE nsIStringInputStream : public nsIInputStream {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISTRINGINPUTSTREAM_IID)

  /* void setData (in string data, in long dataLen); */
  NS_IMETHOD SetData(const char *data, PRInt32 dataLen) = 0;

  /* [noscript] void adoptData (in charPtr data, in long data_len); */
  NS_IMETHOD AdoptData(char * data, PRInt32 data_len) = 0;

  /* [noscript] void shareData (in string data, in long data_len); */
  NS_IMETHOD ShareData(const char *data, PRInt32 data_len) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISTRINGINPUTSTREAM \
  NS_IMETHOD SetData(const char *data, PRInt32 dataLen); \
  NS_IMETHOD AdoptData(char * data, PRInt32 data_len); \
  NS_IMETHOD ShareData(const char *data, PRInt32 data_len); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISTRINGINPUTSTREAM(_to) \
  NS_IMETHOD SetData(const char *data, PRInt32 dataLen) { return _to SetData(data, dataLen); } \
  NS_IMETHOD AdoptData(char * data, PRInt32 data_len) { return _to AdoptData(data, data_len); } \
  NS_IMETHOD ShareData(const char *data, PRInt32 data_len) { return _to ShareData(data, data_len); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISTRINGINPUTSTREAM(_to) \
  NS_IMETHOD SetData(const char *data, PRInt32 dataLen) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetData(data, dataLen); } \
  NS_IMETHOD AdoptData(char * data, PRInt32 data_len) { return !_to ? NS_ERROR_NULL_POINTER : _to->AdoptData(data, data_len); } \
  NS_IMETHOD ShareData(const char *data, PRInt32 data_len) { return !_to ? NS_ERROR_NULL_POINTER : _to->ShareData(data, data_len); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsStringInputStream : public nsIStringInputStream
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISTRINGINPUTSTREAM

  nsStringInputStream();

private:
  ~nsStringInputStream();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsStringInputStream, nsIStringInputStream)

nsStringInputStream::nsStringInputStream()
{
  /* member initializers and constructor code */
}

nsStringInputStream::~nsStringInputStream()
{
  /* destructor code */
}

/* void setData (in string data, in long dataLen); */
NS_IMETHODIMP nsStringInputStream::SetData(const char *data, PRInt32 dataLen)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void adoptData (in charPtr data, in long data_len); */
NS_IMETHODIMP nsStringInputStream::AdoptData(char * data, PRInt32 data_len)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void shareData (in string data, in long data_len); */
NS_IMETHODIMP nsStringInputStream::ShareData(const char *data, PRInt32 data_len)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsITooltipListener */
#define NS_ITOOLTIPLISTENER_IID_STR "44b78386-1dd2-11b2-9ad2-e4eee2ca1916"

#define NS_ITOOLTIPLISTENER_IID \
  {0x44b78386, 0x1dd2, 0x11b2, \
    { 0x9a, 0xd2, 0xe4, 0xee, 0xe2, 0xca, 0x19, 0x16 }}

class NS_NO_VTABLE nsITooltipListener : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ITOOLTIPLISTENER_IID)

  /* void onShowTooltip (in long x_coords, in long y_coords, in wstring tip_text); */
  NS_IMETHOD OnShowTooltip(PRInt32 x_coords, PRInt32 y_coords, const PRUnichar *tip_text) = 0;

  /* void onHideTooltip (); */
  NS_IMETHOD OnHideTooltip(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSITOOLTIPLISTENER \
  NS_IMETHOD OnShowTooltip(PRInt32 x_coords, PRInt32 y_coords, const PRUnichar *tip_text); \
  NS_IMETHOD OnHideTooltip(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSITOOLTIPLISTENER(_to) \
  NS_IMETHOD OnShowTooltip(PRInt32 x_coords, PRInt32 y_coords, const PRUnichar *tip_text) { return _to OnShowTooltip(x_coords, y_coords, tip_text); } \
  NS_IMETHOD OnHideTooltip(void) { return _to OnHideTooltip(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSITOOLTIPLISTENER(_to) \
  NS_IMETHOD OnShowTooltip(PRInt32 x_coords, PRInt32 y_coords, const PRUnichar *tip_text) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnShowTooltip(x_coords, y_coords, tip_text); } \
  NS_IMETHOD OnHideTooltip(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnHideTooltip(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsTooltipListener : public nsITooltipListener
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSITOOLTIPLISTENER

  nsTooltipListener();

private:
  ~nsTooltipListener();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsTooltipListener, nsITooltipListener)

nsTooltipListener::nsTooltipListener()
{
  /* member initializers and constructor code */
}

nsTooltipListener::~nsTooltipListener()
{
  /* destructor code */
}

/* void onShowTooltip (in long x_coords, in long y_coords, in wstring tip_text); */
NS_IMETHODIMP nsTooltipListener::OnShowTooltip(PRInt32 x_coords, PRInt32 y_coords, const PRUnichar *tip_text)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onHideTooltip (); */
NS_IMETHODIMP nsTooltipListener::OnHideTooltip()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIWebProgress; /* forward declaration */

class nsIRequest; /* forward declaration */

class nsIURI; /* forward declaration */


/* starting interface:    nsIWebProgressListener */
#define NS_IWEBPROGRESSLISTENER_IID_STR "570f39d1-efd0-11d3-b093-00a024ffc08c"

#define NS_IWEBPROGRESSLISTENER_IID \
  {0x570f39d1, 0xefd0, 0x11d3, \
    { 0xb0, 0x93, 0x00, 0xa0, 0x24, 0xff, 0xc0, 0x8c }}

class NS_NO_VTABLE nsIWebProgressListener : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBPROGRESSLISTENER_IID)

  enum { STATE_START = 1U };

  enum { STATE_REDIRECTING = 2U };

  enum { STATE_TRANSFERRING = 4U };

  enum { STATE_NEGOTIATING = 8U };

  enum { STATE_STOP = 16U };

  enum { STATE_IS_REQUEST = 65536U };

  enum { STATE_IS_DOCUMENT = 131072U };

  enum { STATE_IS_NETWORK = 262144U };

  enum { STATE_IS_WINDOW = 524288U };

  enum { STATE_RESTORING = 16777216U };

  enum { STATE_IS_INSECURE = 4U };

  enum { STATE_IS_BROKEN = 1U };

  enum { STATE_IS_SECURE = 2U };

  enum { STATE_SECURE_HIGH = 262144U };

  enum { STATE_SECURE_MED = 65536U };

  enum { STATE_SECURE_LOW = 131072U };

  /* void onStateChange (in nsIWebProgress web_progress, in nsIRequest request, in unsigned long state_flags, in nsresult status); */
  NS_IMETHOD OnStateChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state_flags, nsresult status) = 0;

  /* void onProgressChange (in nsIWebProgress web_progress, in nsIRequest request, in long cur_self_progress, in long max_self_progress, in long cur_total_progress, in long max_total_progress); */
  NS_IMETHOD OnProgressChange(nsIWebProgress *web_progress, nsIRequest *request, PRInt32 cur_self_progress, PRInt32 max_self_progress, PRInt32 cur_total_progress, PRInt32 max_total_progress) = 0;

  /* void onLocationChange (in nsIWebProgress web_progress, in nsIRequest request, in nsIURI location); */
  NS_IMETHOD OnLocationChange(nsIWebProgress *web_progress, nsIRequest *request, nsIURI *location) = 0;

  /* void onStatusChange (in nsIWebProgress web_progress, in nsIRequest request, in nsresult status, in wstring message); */
  NS_IMETHOD OnStatusChange(nsIWebProgress *web_progress, nsIRequest *request, nsresult status, const PRUnichar *message) = 0;

  /* void onSecurityChange (in nsIWebProgress web_progress, in nsIRequest request, in unsigned long state); */
  NS_IMETHOD OnSecurityChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBPROGRESSLISTENER \
  NS_IMETHOD OnStateChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state_flags, nsresult status); \
  NS_IMETHOD OnProgressChange(nsIWebProgress *web_progress, nsIRequest *request, PRInt32 cur_self_progress, PRInt32 max_self_progress, PRInt32 cur_total_progress, PRInt32 max_total_progress); \
  NS_IMETHOD OnLocationChange(nsIWebProgress *web_progress, nsIRequest *request, nsIURI *location); \
  NS_IMETHOD OnStatusChange(nsIWebProgress *web_progress, nsIRequest *request, nsresult status, const PRUnichar *message); \
  NS_IMETHOD OnSecurityChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBPROGRESSLISTENER(_to) \
  NS_IMETHOD OnStateChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state_flags, nsresult status) { return _to OnStateChange(web_progress, request, state_flags, status); } \
  NS_IMETHOD OnProgressChange(nsIWebProgress *web_progress, nsIRequest *request, PRInt32 cur_self_progress, PRInt32 max_self_progress, PRInt32 cur_total_progress, PRInt32 max_total_progress) { return _to OnProgressChange(web_progress, request, cur_self_progress, max_self_progress, cur_total_progress, max_total_progress); } \
  NS_IMETHOD OnLocationChange(nsIWebProgress *web_progress, nsIRequest *request, nsIURI *location) { return _to OnLocationChange(web_progress, request, location); } \
  NS_IMETHOD OnStatusChange(nsIWebProgress *web_progress, nsIRequest *request, nsresult status, const PRUnichar *message) { return _to OnStatusChange(web_progress, request, status, message); } \
  NS_IMETHOD OnSecurityChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state) { return _to OnSecurityChange(web_progress, request, state); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBPROGRESSLISTENER(_to) \
  NS_IMETHOD OnStateChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state_flags, nsresult status) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnStateChange(web_progress, request, state_flags, status); } \
  NS_IMETHOD OnProgressChange(nsIWebProgress *web_progress, nsIRequest *request, PRInt32 cur_self_progress, PRInt32 max_self_progress, PRInt32 cur_total_progress, PRInt32 max_total_progress) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnProgressChange(web_progress, request, cur_self_progress, max_self_progress, cur_total_progress, max_total_progress); } \
  NS_IMETHOD OnLocationChange(nsIWebProgress *web_progress, nsIRequest *request, nsIURI *location) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnLocationChange(web_progress, request, location); } \
  NS_IMETHOD OnStatusChange(nsIWebProgress *web_progress, nsIRequest *request, nsresult status, const PRUnichar *message) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnStatusChange(web_progress, request, status, message); } \
  NS_IMETHOD OnSecurityChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnSecurityChange(web_progress, request, state); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebProgressListener : public nsIWebProgressListener
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBPROGRESSLISTENER

  nsWebProgressListener();

private:
  ~nsWebProgressListener();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebProgressListener, nsIWebProgressListener)

nsWebProgressListener::nsWebProgressListener()
{
  /* member initializers and constructor code */
}

nsWebProgressListener::~nsWebProgressListener()
{
  /* destructor code */
}

/* void onStateChange (in nsIWebProgress web_progress, in nsIRequest request, in unsigned long state_flags, in nsresult status); */
NS_IMETHODIMP nsWebProgressListener::OnStateChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state_flags, nsresult status)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onProgressChange (in nsIWebProgress web_progress, in nsIRequest request, in long cur_self_progress, in long max_self_progress, in long cur_total_progress, in long max_total_progress); */
NS_IMETHODIMP nsWebProgressListener::OnProgressChange(nsIWebProgress *web_progress, nsIRequest *request, PRInt32 cur_self_progress, PRInt32 max_self_progress, PRInt32 cur_total_progress, PRInt32 max_total_progress)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onLocationChange (in nsIWebProgress web_progress, in nsIRequest request, in nsIURI location); */
NS_IMETHODIMP nsWebProgressListener::OnLocationChange(nsIWebProgress *web_progress, nsIRequest *request, nsIURI *location)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onStatusChange (in nsIWebProgress web_progress, in nsIRequest request, in nsresult status, in wstring message); */
NS_IMETHODIMP nsWebProgressListener::OnStatusChange(nsIWebProgress *web_progress, nsIRequest *request, nsresult status, const PRUnichar *message)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onSecurityChange (in nsIWebProgress web_progress, in nsIRequest request, in unsigned long state); */
NS_IMETHODIMP nsWebProgressListener::OnSecurityChange(nsIWebProgress *web_progress, nsIRequest *request, PRUint32 state)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIWebProgressListener2 */
#define NS_IWEBPROGRESSLISTENER2_IID_STR "3f24610d-1e1f-4151-9d2e-239884742324"

#define NS_IWEBPROGRESSLISTENER2_IID \
  {0x3f24610d, 0x1e1f, 0x4151, \
    { 0x9d, 0x2e, 0x23, 0x98, 0x84, 0x74, 0x23, 0x24 }}

class NS_NO_VTABLE nsIWebProgressListener2 : public nsIWebProgressListener {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBPROGRESSLISTENER2_IID)

  /* void onProgressChange64 (in nsIWebProgress web_progress, in nsIRequest request, in long long cur_self_progress, in long long max_self_progress, in long long cur_total_progress, in long long max_total_progress); */
  NS_IMETHOD OnProgressChange64(nsIWebProgress *web_progress, nsIRequest *request, PRInt64 cur_self_progress, PRInt64 max_self_progress, PRInt64 cur_total_progress, PRInt64 max_total_progress) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBPROGRESSLISTENER2 \
  NS_IMETHOD OnProgressChange64(nsIWebProgress *web_progress, nsIRequest *request, PRInt64 cur_self_progress, PRInt64 max_self_progress, PRInt64 cur_total_progress, PRInt64 max_total_progress); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBPROGRESSLISTENER2(_to) \
  NS_IMETHOD OnProgressChange64(nsIWebProgress *web_progress, nsIRequest *request, PRInt64 cur_self_progress, PRInt64 max_self_progress, PRInt64 cur_total_progress, PRInt64 max_total_progress) { return _to OnProgressChange64(web_progress, request, cur_self_progress, max_self_progress, cur_total_progress, max_total_progress); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBPROGRESSLISTENER2(_to) \
  NS_IMETHOD OnProgressChange64(nsIWebProgress *web_progress, nsIRequest *request, PRInt64 cur_self_progress, PRInt64 max_self_progress, PRInt64 cur_total_progress, PRInt64 max_total_progress) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnProgressChange64(web_progress, request, cur_self_progress, max_self_progress, cur_total_progress, max_total_progress); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebProgressListener2 : public nsIWebProgressListener2
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBPROGRESSLISTENER2

  nsWebProgressListener2();

private:
  ~nsWebProgressListener2();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebProgressListener2, nsIWebProgressListener2)

nsWebProgressListener2::nsWebProgressListener2()
{
  /* member initializers and constructor code */
}

nsWebProgressListener2::~nsWebProgressListener2()
{
  /* destructor code */
}

/* void onProgressChange64 (in nsIWebProgress web_progress, in nsIRequest request, in long long cur_self_progress, in long long max_self_progress, in long long cur_total_progress, in long long max_total_progress); */
NS_IMETHODIMP nsWebProgressListener2::OnProgressChange64(nsIWebProgress *web_progress, nsIRequest *request, PRInt64 cur_self_progress, PRInt64 max_self_progress, PRInt64 cur_total_progress, PRInt64 max_total_progress)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIMIMEInfo; /* forward declaration */

class nsILocalFile; /* forward declaration */

class nsICancelable; /* forward declaration */


/* starting interface:    nsITransfer */
#define NS_ITRANSFER_IID_STR "23c51569-e9a1-4a92-adeb-3723db82ef7c"

#define NS_ITRANSFER_IID \
  {0x23c51569, 0xe9a1, 0x4a92, \
    { 0xad, 0xeb, 0x37, 0x23, 0xdb, 0x82, 0xef, 0x7c }}

class NS_NO_VTABLE nsITransfer : public nsIWebProgressListener2 {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ITRANSFER_IID)

  /* void init (in nsIURI source, in nsIURI target, in AString display_name, in nsIMIMEInfo mime_info, in PRTime start_time, in nsILocalFile temp_file, in nsICancelable cancelable); */
  NS_IMETHOD Init(nsIURI *source, nsIURI *target, const nsAString & display_name, nsIMIMEInfo *mime_info, PRTime start_time, nsILocalFile *temp_file, nsICancelable *cancelable) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSITRANSFER \
  NS_IMETHOD Init(nsIURI *source, nsIURI *target, const nsAString & display_name, nsIMIMEInfo *mime_info, PRTime start_time, nsILocalFile *temp_file, nsICancelable *cancelable); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSITRANSFER(_to) \
  NS_IMETHOD Init(nsIURI *source, nsIURI *target, const nsAString & display_name, nsIMIMEInfo *mime_info, PRTime start_time, nsILocalFile *temp_file, nsICancelable *cancelable) { return _to Init(source, target, display_name, mime_info, start_time, temp_file, cancelable); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSITRANSFER(_to) \
  NS_IMETHOD Init(nsIURI *source, nsIURI *target, const nsAString & display_name, nsIMIMEInfo *mime_info, PRTime start_time, nsILocalFile *temp_file, nsICancelable *cancelable) { return !_to ? NS_ERROR_NULL_POINTER : _to->Init(source, target, display_name, mime_info, start_time, temp_file, cancelable); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsTransfer : public nsITransfer
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSITRANSFER

  nsTransfer();

private:
  ~nsTransfer();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsTransfer, nsITransfer)

nsTransfer::nsTransfer()
{
  /* member initializers and constructor code */
}

nsTransfer::~nsTransfer()
{
  /* destructor code */
}

/* void init (in nsIURI source, in nsIURI target, in AString display_name, in nsIMIMEInfo mime_info, in PRTime start_time, in nsILocalFile temp_file, in nsICancelable cancelable); */
NS_IMETHODIMP nsTransfer::Init(nsIURI *source, nsIURI *target, const nsAString & display_name, nsIMIMEInfo *mime_info, PRTime start_time, nsILocalFile *temp_file, nsICancelable *cancelable)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIURI */
#define NS_IURI_IID_STR "07a22cc0-0ce5-11d3-9331-00104ba0fd40"

#define NS_IURI_IID \
  {0x07a22cc0, 0x0ce5, 0x11d3, \
    { 0x93, 0x31, 0x00, 0x10, 0x4b, 0xa0, 0xfd, 0x40 }}

class NS_NO_VTABLE nsIURI : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IURI_IID)

  /* attribute AUTF8String spec; */
  NS_IMETHOD GetSpec(nsACString & aSpec) = 0;
  NS_IMETHOD SetSpec(const nsACString & aSpec) = 0;

  /* readonly attribute AUTF8String prePath; */
  NS_IMETHOD GetPrePath(nsACString & aPrePath) = 0;

  /* attribute ACString scheme; */
  NS_IMETHOD GetScheme(nsACString & aScheme) = 0;
  NS_IMETHOD SetScheme(const nsACString & aScheme) = 0;

  /* attribute AUTF8String userPass; */
  NS_IMETHOD GetUserPass(nsACString & aUserPass) = 0;
  NS_IMETHOD SetUserPass(const nsACString & aUserPass) = 0;

  /* attribute AUTF8String username; */
  NS_IMETHOD GetUsername(nsACString & aUsername) = 0;
  NS_IMETHOD SetUsername(const nsACString & aUsername) = 0;

  /* attribute AUTF8String password; */
  NS_IMETHOD GetPassword(nsACString & aPassword) = 0;
  NS_IMETHOD SetPassword(const nsACString & aPassword) = 0;

  /* attribute AUTF8String hostPort; */
  NS_IMETHOD GetHostPort(nsACString & aHostPort) = 0;
  NS_IMETHOD SetHostPort(const nsACString & aHostPort) = 0;

  /* attribute AUTF8String host; */
  NS_IMETHOD GetHost(nsACString & aHost) = 0;
  NS_IMETHOD SetHost(const nsACString & aHost) = 0;

  /* attribute long port; */
  NS_IMETHOD GetPort(PRInt32 *aPort) = 0;
  NS_IMETHOD SetPort(PRInt32 aPort) = 0;

  /* attribute AUTF8String path; */
  NS_IMETHOD GetPath(nsACString & aPath) = 0;
  NS_IMETHOD SetPath(const nsACString & aPath) = 0;

  /* boolean equals (in nsIURI other); */
  NS_IMETHOD Equals(nsIURI *other, PRBool *_retval) = 0;

  /* boolean schemeIs (in string scheme); */
  NS_IMETHOD SchemeIs(const char *scheme, PRBool *_retval) = 0;

  /* nsIURI clone (); */
  NS_IMETHOD Clone(nsIURI **_retval) = 0;

  /* AUTF8String resolve (in AUTF8String relative_path); */
  NS_IMETHOD Resolve(const nsACString & relative_path, nsACString & _retval) = 0;

  /* readonly attribute ACString asciiSpec; */
  NS_IMETHOD GetAsciiSpec(nsACString & aAsciiSpec) = 0;

  /* readonly attribute ACString asciiHost; */
  NS_IMETHOD GetAsciiHost(nsACString & aAsciiHost) = 0;

  /* readonly attribute ACString originCharset; */
  NS_IMETHOD GetOriginCharset(nsACString & aOriginCharset) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIURI \
  NS_IMETHOD GetSpec(nsACString & aSpec); \
  NS_IMETHOD SetSpec(const nsACString & aSpec); \
  NS_IMETHOD GetPrePath(nsACString & aPrePath); \
  NS_IMETHOD GetScheme(nsACString & aScheme); \
  NS_IMETHOD SetScheme(const nsACString & aScheme); \
  NS_IMETHOD GetUserPass(nsACString & aUserPass); \
  NS_IMETHOD SetUserPass(const nsACString & aUserPass); \
  NS_IMETHOD GetUsername(nsACString & aUsername); \
  NS_IMETHOD SetUsername(const nsACString & aUsername); \
  NS_IMETHOD GetPassword(nsACString & aPassword); \
  NS_IMETHOD SetPassword(const nsACString & aPassword); \
  NS_IMETHOD GetHostPort(nsACString & aHostPort); \
  NS_IMETHOD SetHostPort(const nsACString & aHostPort); \
  NS_IMETHOD GetHost(nsACString & aHost); \
  NS_IMETHOD SetHost(const nsACString & aHost); \
  NS_IMETHOD GetPort(PRInt32 *aPort); \
  NS_IMETHOD SetPort(PRInt32 aPort); \
  NS_IMETHOD GetPath(nsACString & aPath); \
  NS_IMETHOD SetPath(const nsACString & aPath); \
  NS_IMETHOD Equals(nsIURI *other, PRBool *_retval); \
  NS_IMETHOD SchemeIs(const char *scheme, PRBool *_retval); \
  NS_IMETHOD Clone(nsIURI **_retval); \
  NS_IMETHOD Resolve(const nsACString & relative_path, nsACString & _retval); \
  NS_IMETHOD GetAsciiSpec(nsACString & aAsciiSpec); \
  NS_IMETHOD GetAsciiHost(nsACString & aAsciiHost); \
  NS_IMETHOD GetOriginCharset(nsACString & aOriginCharset); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIURI(_to) \
  NS_IMETHOD GetSpec(nsACString & aSpec) { return _to GetSpec(aSpec); } \
  NS_IMETHOD SetSpec(const nsACString & aSpec) { return _to SetSpec(aSpec); } \
  NS_IMETHOD GetPrePath(nsACString & aPrePath) { return _to GetPrePath(aPrePath); } \
  NS_IMETHOD GetScheme(nsACString & aScheme) { return _to GetScheme(aScheme); } \
  NS_IMETHOD SetScheme(const nsACString & aScheme) { return _to SetScheme(aScheme); } \
  NS_IMETHOD GetUserPass(nsACString & aUserPass) { return _to GetUserPass(aUserPass); } \
  NS_IMETHOD SetUserPass(const nsACString & aUserPass) { return _to SetUserPass(aUserPass); } \
  NS_IMETHOD GetUsername(nsACString & aUsername) { return _to GetUsername(aUsername); } \
  NS_IMETHOD SetUsername(const nsACString & aUsername) { return _to SetUsername(aUsername); } \
  NS_IMETHOD GetPassword(nsACString & aPassword) { return _to GetPassword(aPassword); } \
  NS_IMETHOD SetPassword(const nsACString & aPassword) { return _to SetPassword(aPassword); } \
  NS_IMETHOD GetHostPort(nsACString & aHostPort) { return _to GetHostPort(aHostPort); } \
  NS_IMETHOD SetHostPort(const nsACString & aHostPort) { return _to SetHostPort(aHostPort); } \
  NS_IMETHOD GetHost(nsACString & aHost) { return _to GetHost(aHost); } \
  NS_IMETHOD SetHost(const nsACString & aHost) { return _to SetHost(aHost); } \
  NS_IMETHOD GetPort(PRInt32 *aPort) { return _to GetPort(aPort); } \
  NS_IMETHOD SetPort(PRInt32 aPort) { return _to SetPort(aPort); } \
  NS_IMETHOD GetPath(nsACString & aPath) { return _to GetPath(aPath); } \
  NS_IMETHOD SetPath(const nsACString & aPath) { return _to SetPath(aPath); } \
  NS_IMETHOD Equals(nsIURI *other, PRBool *_retval) { return _to Equals(other, _retval); } \
  NS_IMETHOD SchemeIs(const char *scheme, PRBool *_retval) { return _to SchemeIs(scheme, _retval); } \
  NS_IMETHOD Clone(nsIURI **_retval) { return _to Clone(_retval); } \
  NS_IMETHOD Resolve(const nsACString & relative_path, nsACString & _retval) { return _to Resolve(relative_path, _retval); } \
  NS_IMETHOD GetAsciiSpec(nsACString & aAsciiSpec) { return _to GetAsciiSpec(aAsciiSpec); } \
  NS_IMETHOD GetAsciiHost(nsACString & aAsciiHost) { return _to GetAsciiHost(aAsciiHost); } \
  NS_IMETHOD GetOriginCharset(nsACString & aOriginCharset) { return _to GetOriginCharset(aOriginCharset); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIURI(_to) \
  NS_IMETHOD GetSpec(nsACString & aSpec) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSpec(aSpec); } \
  NS_IMETHOD SetSpec(const nsACString & aSpec) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSpec(aSpec); } \
  NS_IMETHOD GetPrePath(nsACString & aPrePath) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrePath(aPrePath); } \
  NS_IMETHOD GetScheme(nsACString & aScheme) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetScheme(aScheme); } \
  NS_IMETHOD SetScheme(const nsACString & aScheme) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetScheme(aScheme); } \
  NS_IMETHOD GetUserPass(nsACString & aUserPass) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetUserPass(aUserPass); } \
  NS_IMETHOD SetUserPass(const nsACString & aUserPass) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetUserPass(aUserPass); } \
  NS_IMETHOD GetUsername(nsACString & aUsername) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetUsername(aUsername); } \
  NS_IMETHOD SetUsername(const nsACString & aUsername) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetUsername(aUsername); } \
  NS_IMETHOD GetPassword(nsACString & aPassword) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPassword(aPassword); } \
  NS_IMETHOD SetPassword(const nsACString & aPassword) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPassword(aPassword); } \
  NS_IMETHOD GetHostPort(nsACString & aHostPort) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHostPort(aHostPort); } \
  NS_IMETHOD SetHostPort(const nsACString & aHostPort) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHostPort(aHostPort); } \
  NS_IMETHOD GetHost(nsACString & aHost) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHost(aHost); } \
  NS_IMETHOD SetHost(const nsACString & aHost) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHost(aHost); } \
  NS_IMETHOD GetPort(PRInt32 *aPort) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPort(aPort); } \
  NS_IMETHOD SetPort(PRInt32 aPort) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPort(aPort); } \
  NS_IMETHOD GetPath(nsACString & aPath) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPath(aPath); } \
  NS_IMETHOD SetPath(const nsACString & aPath) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPath(aPath); } \
  NS_IMETHOD Equals(nsIURI *other, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Equals(other, _retval); } \
  NS_IMETHOD SchemeIs(const char *scheme, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SchemeIs(scheme, _retval); } \
  NS_IMETHOD Clone(nsIURI **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Clone(_retval); } \
  NS_IMETHOD Resolve(const nsACString & relative_path, nsACString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Resolve(relative_path, _retval); } \
  NS_IMETHOD GetAsciiSpec(nsACString & aAsciiSpec) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAsciiSpec(aAsciiSpec); } \
  NS_IMETHOD GetAsciiHost(nsACString & aAsciiHost) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAsciiHost(aAsciiHost); } \
  NS_IMETHOD GetOriginCharset(nsACString & aOriginCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetOriginCharset(aOriginCharset); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsURI : public nsIURI
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIURI

  nsURI();

private:
  ~nsURI();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsURI, nsIURI)

nsURI::nsURI()
{
  /* member initializers and constructor code */
}

nsURI::~nsURI()
{
  /* destructor code */
}

/* attribute AUTF8String spec; */
NS_IMETHODIMP nsURI::GetSpec(nsACString & aSpec)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetSpec(const nsACString & aSpec)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AUTF8String prePath; */
NS_IMETHODIMP nsURI::GetPrePath(nsACString & aPrePath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute ACString scheme; */
NS_IMETHODIMP nsURI::GetScheme(nsACString & aScheme)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetScheme(const nsACString & aScheme)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AUTF8String userPass; */
NS_IMETHODIMP nsURI::GetUserPass(nsACString & aUserPass)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetUserPass(const nsACString & aUserPass)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AUTF8String username; */
NS_IMETHODIMP nsURI::GetUsername(nsACString & aUsername)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetUsername(const nsACString & aUsername)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AUTF8String password; */
NS_IMETHODIMP nsURI::GetPassword(nsACString & aPassword)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetPassword(const nsACString & aPassword)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AUTF8String hostPort; */
NS_IMETHODIMP nsURI::GetHostPort(nsACString & aHostPort)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetHostPort(const nsACString & aHostPort)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AUTF8String host; */
NS_IMETHODIMP nsURI::GetHost(nsACString & aHost)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetHost(const nsACString & aHost)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long port; */
NS_IMETHODIMP nsURI::GetPort(PRInt32 *aPort)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetPort(PRInt32 aPort)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute AUTF8String path; */
NS_IMETHODIMP nsURI::GetPath(nsACString & aPath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURI::SetPath(const nsACString & aPath)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean equals (in nsIURI other); */
NS_IMETHODIMP nsURI::Equals(nsIURI *other, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean schemeIs (in string scheme); */
NS_IMETHODIMP nsURI::SchemeIs(const char *scheme, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIURI clone (); */
NS_IMETHODIMP nsURI::Clone(nsIURI **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AUTF8String resolve (in AUTF8String relative_path); */
NS_IMETHODIMP nsURI::Resolve(const nsACString & relative_path, nsACString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute ACString asciiSpec; */
NS_IMETHODIMP nsURI::GetAsciiSpec(nsACString & aAsciiSpec)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute ACString asciiHost; */
NS_IMETHODIMP nsURI::GetAsciiHost(nsACString & aAsciiHost)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute ACString originCharset; */
NS_IMETHODIMP nsURI::GetOriginCharset(nsACString & aOriginCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIURI; /* forward declaration */

class nsIRequest; /* forward declaration */

class nsIStreamListener; /* forward declaration */

class nsIURIContentListener; /* forward declaration */


/* starting interface:    nsIURIContentListener */
#define NS_IURICONTENTLISTENER_IID_STR "94928ab3-8b63-11d3-989d-001083010e9b"

#define NS_IURICONTENTLISTENER_IID \
  {0x94928ab3, 0x8b63, 0x11d3, \
    { 0x98, 0x9d, 0x00, 0x10, 0x83, 0x01, 0x0e, 0x9b }}

class NS_NO_VTABLE nsIURIContentListener : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IURICONTENTLISTENER_IID)

  /* boolean onStartURIOpen (in nsIURI uri); */
  NS_IMETHOD OnStartURIOpen(nsIURI *uri, PRBool *_retval) = 0;

  /* boolean doContent (in string content_type, in boolean is_content_preferred, in nsIRequest request, out nsIStreamListener content_handler); */
  NS_IMETHOD DoContent(const char *content_type, PRBool is_content_preferred, nsIRequest *request, nsIStreamListener **content_handler, PRBool *_retval) = 0;

  /* boolean isPreferred (in string content_type, out string desired_content_type); */
  NS_IMETHOD IsPreferred(const char *content_type, char **desired_content_type, PRBool *_retval) = 0;

  /* boolean canHandleContent (in string content_type, in boolean is_content_preferred, out string desired_content_type); */
  NS_IMETHOD CanHandleContent(const char *content_type, PRBool is_content_preferred, char **desired_content_type, PRBool *_retval) = 0;

  /* attribute nsISupports loadCookie; */
  NS_IMETHOD GetLoadCookie(nsISupports * *aLoadCookie) = 0;
  NS_IMETHOD SetLoadCookie(nsISupports * aLoadCookie) = 0;

  /* attribute nsIURIContentListener parentContentListener; */
  NS_IMETHOD GetParentContentListener(nsIURIContentListener * *aParentContentListener) = 0;
  NS_IMETHOD SetParentContentListener(nsIURIContentListener * aParentContentListener) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIURICONTENTLISTENER \
  NS_IMETHOD OnStartURIOpen(nsIURI *uri, PRBool *_retval); \
  NS_IMETHOD DoContent(const char *content_type, PRBool is_content_preferred, nsIRequest *request, nsIStreamListener **content_handler, PRBool *_retval); \
  NS_IMETHOD IsPreferred(const char *content_type, char **desired_content_type, PRBool *_retval); \
  NS_IMETHOD CanHandleContent(const char *content_type, PRBool is_content_preferred, char **desired_content_type, PRBool *_retval); \
  NS_IMETHOD GetLoadCookie(nsISupports * *aLoadCookie); \
  NS_IMETHOD SetLoadCookie(nsISupports * aLoadCookie); \
  NS_IMETHOD GetParentContentListener(nsIURIContentListener * *aParentContentListener); \
  NS_IMETHOD SetParentContentListener(nsIURIContentListener * aParentContentListener); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIURICONTENTLISTENER(_to) \
  NS_IMETHOD OnStartURIOpen(nsIURI *uri, PRBool *_retval) { return _to OnStartURIOpen(uri, _retval); } \
  NS_IMETHOD DoContent(const char *content_type, PRBool is_content_preferred, nsIRequest *request, nsIStreamListener **content_handler, PRBool *_retval) { return _to DoContent(content_type, is_content_preferred, request, content_handler, _retval); } \
  NS_IMETHOD IsPreferred(const char *content_type, char **desired_content_type, PRBool *_retval) { return _to IsPreferred(content_type, desired_content_type, _retval); } \
  NS_IMETHOD CanHandleContent(const char *content_type, PRBool is_content_preferred, char **desired_content_type, PRBool *_retval) { return _to CanHandleContent(content_type, is_content_preferred, desired_content_type, _retval); } \
  NS_IMETHOD GetLoadCookie(nsISupports * *aLoadCookie) { return _to GetLoadCookie(aLoadCookie); } \
  NS_IMETHOD SetLoadCookie(nsISupports * aLoadCookie) { return _to SetLoadCookie(aLoadCookie); } \
  NS_IMETHOD GetParentContentListener(nsIURIContentListener * *aParentContentListener) { return _to GetParentContentListener(aParentContentListener); } \
  NS_IMETHOD SetParentContentListener(nsIURIContentListener * aParentContentListener) { return _to SetParentContentListener(aParentContentListener); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIURICONTENTLISTENER(_to) \
  NS_IMETHOD OnStartURIOpen(nsIURI *uri, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnStartURIOpen(uri, _retval); } \
  NS_IMETHOD DoContent(const char *content_type, PRBool is_content_preferred, nsIRequest *request, nsIStreamListener **content_handler, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->DoContent(content_type, is_content_preferred, request, content_handler, _retval); } \
  NS_IMETHOD IsPreferred(const char *content_type, char **desired_content_type, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsPreferred(content_type, desired_content_type, _retval); } \
  NS_IMETHOD CanHandleContent(const char *content_type, PRBool is_content_preferred, char **desired_content_type, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanHandleContent(content_type, is_content_preferred, desired_content_type, _retval); } \
  NS_IMETHOD GetLoadCookie(nsISupports * *aLoadCookie) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLoadCookie(aLoadCookie); } \
  NS_IMETHOD SetLoadCookie(nsISupports * aLoadCookie) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetLoadCookie(aLoadCookie); } \
  NS_IMETHOD GetParentContentListener(nsIURIContentListener * *aParentContentListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParentContentListener(aParentContentListener); } \
  NS_IMETHOD SetParentContentListener(nsIURIContentListener * aParentContentListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetParentContentListener(aParentContentListener); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsURIContentListener : public nsIURIContentListener
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIURICONTENTLISTENER

  nsURIContentListener();

private:
  ~nsURIContentListener();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsURIContentListener, nsIURIContentListener)

nsURIContentListener::nsURIContentListener()
{
  /* member initializers and constructor code */
}

nsURIContentListener::~nsURIContentListener()
{
  /* destructor code */
}

/* boolean onStartURIOpen (in nsIURI uri); */
NS_IMETHODIMP nsURIContentListener::OnStartURIOpen(nsIURI *uri, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean doContent (in string content_type, in boolean is_content_preferred, in nsIRequest request, out nsIStreamListener content_handler); */
NS_IMETHODIMP nsURIContentListener::DoContent(const char *content_type, PRBool is_content_preferred, nsIRequest *request, nsIStreamListener **content_handler, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isPreferred (in string content_type, out string desired_content_type); */
NS_IMETHODIMP nsURIContentListener::IsPreferred(const char *content_type, char **desired_content_type, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean canHandleContent (in string content_type, in boolean is_content_preferred, out string desired_content_type); */
NS_IMETHODIMP nsURIContentListener::CanHandleContent(const char *content_type, PRBool is_content_preferred, char **desired_content_type, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsISupports loadCookie; */
NS_IMETHODIMP nsURIContentListener::GetLoadCookie(nsISupports * *aLoadCookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURIContentListener::SetLoadCookie(nsISupports * aLoadCookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIURIContentListener parentContentListener; */
NS_IMETHODIMP nsURIContentListener::GetParentContentListener(nsIURIContentListener * *aParentContentListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsURIContentListener::SetParentContentListener(nsIURIContentListener * aParentContentListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIURIContentListener; /* forward declaration */

class nsIChannel; /* forward declaration */

class nsIInterfaceRequestor; /* forward declaration */

class nsIStreamListener; /* forward declaration */


/* starting interface:    nsIURILoader */
#define NS_IURILOADER_IID_STR "2f7e8051-f1c9-4bcc-8584-9cfd5849e343"

#define NS_IURILOADER_IID \
  {0x2f7e8051, 0xf1c9, 0x4bcc, \
    { 0x85, 0x84, 0x9c, 0xfd, 0x58, 0x49, 0xe3, 0x43 }}

class NS_NO_VTABLE nsIURILoader : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IURILOADER_IID)

  enum { IS_CONTENT_PREFERRED = 1U };

  enum { DONT_RETARGET = 2U };

  /* void registerContentListener (in nsIURIContentListener content_listener); */
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener) = 0;

  /* void unRegisterContentListener (in nsIURIContentListener content_listener); */
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener) = 0;

  /* void openURI (in nsIChannel channel, in boolean is_content_preferred, in nsIInterfaceRequestor window_context); */
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context) = 0;

  /* nsIStreamListener openChannel (in nsIChannel channel, in unsigned long flags, in nsIInterfaceRequestor window_context); */
  NS_IMETHOD OpenChannel(nsIChannel *channel, PRUint32 flags, nsIInterfaceRequestor *window_context, nsIStreamListener **_retval) = 0;

  /* void stop (in nsISupports load_cookie); */
  NS_IMETHOD Stop(nsISupports *load_cookie) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIURILOADER \
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener); \
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener); \
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context); \
  NS_IMETHOD OpenChannel(nsIChannel *channel, PRUint32 flags, nsIInterfaceRequestor *window_context, nsIStreamListener **_retval); \
  NS_IMETHOD Stop(nsISupports *load_cookie); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIURILOADER(_to) \
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener) { return _to RegisterContentListener(content_listener); } \
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener) { return _to UnRegisterContentListener(content_listener); } \
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context) { return _to OpenURI(channel, is_content_preferred, window_context); } \
  NS_IMETHOD OpenChannel(nsIChannel *channel, PRUint32 flags, nsIInterfaceRequestor *window_context, nsIStreamListener **_retval) { return _to OpenChannel(channel, flags, window_context, _retval); } \
  NS_IMETHOD Stop(nsISupports *load_cookie) { return _to Stop(load_cookie); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIURILOADER(_to) \
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener) { return !_to ? NS_ERROR_NULL_POINTER : _to->RegisterContentListener(content_listener); } \
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnRegisterContentListener(content_listener); } \
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenURI(channel, is_content_preferred, window_context); } \
  NS_IMETHOD OpenChannel(nsIChannel *channel, PRUint32 flags, nsIInterfaceRequestor *window_context, nsIStreamListener **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenChannel(channel, flags, window_context, _retval); } \
  NS_IMETHOD Stop(nsISupports *load_cookie) { return !_to ? NS_ERROR_NULL_POINTER : _to->Stop(load_cookie); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsURILoader : public nsIURILoader
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIURILOADER

  nsURILoader();

private:
  ~nsURILoader();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsURILoader, nsIURILoader)

nsURILoader::nsURILoader()
{
  /* member initializers and constructor code */
}

nsURILoader::~nsURILoader()
{
  /* destructor code */
}

/* void registerContentListener (in nsIURIContentListener content_listener); */
NS_IMETHODIMP nsURILoader::RegisterContentListener(nsIURIContentListener *content_listener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void unRegisterContentListener (in nsIURIContentListener content_listener); */
NS_IMETHODIMP nsURILoader::UnRegisterContentListener(nsIURIContentListener *content_listener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void openURI (in nsIChannel channel, in boolean is_content_preferred, in nsIInterfaceRequestor window_context); */
NS_IMETHODIMP nsURILoader::OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIStreamListener openChannel (in nsIChannel channel, in unsigned long flags, in nsIInterfaceRequestor window_context); */
NS_IMETHODIMP nsURILoader::OpenChannel(nsIChannel *channel, PRUint32 flags, nsIInterfaceRequestor *window_context, nsIStreamListener **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void stop (in nsISupports load_cookie); */
NS_IMETHODIMP nsURILoader::Stop(nsISupports *load_cookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    ns18IURILoader */
#define NS_18IURILOADER_IID_STR "5cf6420c-74f3-4a7c-bc1d-f5756d79ea07"

#define NS_18IURILOADER_IID \
  {0x5cf6420c, 0x74f3, 0x4a7c, \
    { 0xbc, 0x1d, 0xf5, 0x75, 0x6d, 0x79, 0xea, 0x07 }}

class NS_NO_VTABLE ns18IURILoader : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_18IURILOADER_IID)

  /* void registerContentListener (in nsIURIContentListener content_listener); */
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener) = 0;

  /* void unRegisterContentListener (in nsIURIContentListener content_listener); */
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener) = 0;

  /* void openURI (in nsIChannel channel, in boolean is_content_preferred, in nsIInterfaceRequestor window_context); */
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context) = 0;

  /* void stop (in nsISupports load_cookie); */
  NS_IMETHOD Stop(nsISupports *load_cookie) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NS18IURILOADER \
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener); \
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener); \
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context); \
  NS_IMETHOD Stop(nsISupports *load_cookie); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NS18IURILOADER(_to) \
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener) { return _to RegisterContentListener(content_listener); } \
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener) { return _to UnRegisterContentListener(content_listener); } \
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context) { return _to OpenURI(channel, is_content_preferred, window_context); } \
  NS_IMETHOD Stop(nsISupports *load_cookie) { return _to Stop(load_cookie); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NS18IURILOADER(_to) \
  NS_IMETHOD RegisterContentListener(nsIURIContentListener *content_listener) { return !_to ? NS_ERROR_NULL_POINTER : _to->RegisterContentListener(content_listener); } \
  NS_IMETHOD UnRegisterContentListener(nsIURIContentListener *content_listener) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnRegisterContentListener(content_listener); } \
  NS_IMETHOD OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenURI(channel, is_content_preferred, window_context); } \
  NS_IMETHOD Stop(nsISupports *load_cookie) { return !_to ? NS_ERROR_NULL_POINTER : _to->Stop(load_cookie); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class _MYCLASS_ : public ns18IURILoader
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NS18IURILOADER

  _MYCLASS_();

private:
  ~_MYCLASS_();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(_MYCLASS_, ns18IURILoader)

_MYCLASS_::_MYCLASS_()
{
  /* member initializers and constructor code */
}

_MYCLASS_::~_MYCLASS_()
{
  /* destructor code */
}

/* void registerContentListener (in nsIURIContentListener content_listener); */
NS_IMETHODIMP _MYCLASS_::RegisterContentListener(nsIURIContentListener *content_listener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void unRegisterContentListener (in nsIURIContentListener content_listener); */
NS_IMETHODIMP _MYCLASS_::UnRegisterContentListener(nsIURIContentListener *content_listener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void openURI (in nsIChannel channel, in boolean is_content_preferred, in nsIInterfaceRequestor window_context); */
NS_IMETHODIMP _MYCLASS_::OpenURI(nsIChannel *channel, PRBool is_content_preferred, nsIInterfaceRequestor *window_context)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void stop (in nsISupports load_cookie); */
NS_IMETHODIMP _MYCLASS_::Stop(nsISupports *load_cookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIWebBrowserChrome; /* forward declaration */

class nsIURIContentListener; /* forward declaration */

class nsIDOMWindow; /* forward declaration */

class nsIWeakReference; /* forward declaration */


/* starting interface:    nsIWebBrowser */
#define NS_IWEBBROWSER_IID_STR "69e5df00-7b8b-11d3-af61-00a024ffc08c"

#define NS_IWEBBROWSER_IID \
  {0x69e5df00, 0x7b8b, 0x11d3, \
    { 0xaf, 0x61, 0x00, 0xa0, 0x24, 0xff, 0xc0, 0x8c }}

class NS_NO_VTABLE nsIWebBrowser : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBBROWSER_IID)

  /* void addWebBrowserListener (in nsIWeakReference listener, in nsIIDRef iid); */
  NS_IMETHOD AddWebBrowserListener(nsIWeakReference *listener, const nsIID & iid) = 0;

  /* void removeWebBrowserListener (in nsIWeakReference listener, in nsIIDRef iid); */
  NS_IMETHOD RemoveWebBrowserListener(nsIWeakReference *listener, const nsIID & iid) = 0;

  /* attribute nsIWebBrowserChrome containerWindow; */
  NS_IMETHOD GetContainerWindow(nsIWebBrowserChrome * *aContainerWindow) = 0;
  NS_IMETHOD SetContainerWindow(nsIWebBrowserChrome * aContainerWindow) = 0;

  /* attribute nsIURIContentListener parentURIContentListener; */
  NS_IMETHOD GetParentURIContentListener(nsIURIContentListener * *aParentURIContentListener) = 0;
  NS_IMETHOD SetParentURIContentListener(nsIURIContentListener * aParentURIContentListener) = 0;

  /* readonly attribute nsIDOMWindow contentDOMWindow; */
  NS_IMETHOD GetContentDOMWindow(nsIDOMWindow * *aContentDOMWindow) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBBROWSER \
  NS_IMETHOD AddWebBrowserListener(nsIWeakReference *listener, const nsIID & iid); \
  NS_IMETHOD RemoveWebBrowserListener(nsIWeakReference *listener, const nsIID & iid); \
  NS_IMETHOD GetContainerWindow(nsIWebBrowserChrome * *aContainerWindow); \
  NS_IMETHOD SetContainerWindow(nsIWebBrowserChrome * aContainerWindow); \
  NS_IMETHOD GetParentURIContentListener(nsIURIContentListener * *aParentURIContentListener); \
  NS_IMETHOD SetParentURIContentListener(nsIURIContentListener * aParentURIContentListener); \
  NS_IMETHOD GetContentDOMWindow(nsIDOMWindow * *aContentDOMWindow); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBBROWSER(_to) \
  NS_IMETHOD AddWebBrowserListener(nsIWeakReference *listener, const nsIID & iid) { return _to AddWebBrowserListener(listener, iid); } \
  NS_IMETHOD RemoveWebBrowserListener(nsIWeakReference *listener, const nsIID & iid) { return _to RemoveWebBrowserListener(listener, iid); } \
  NS_IMETHOD GetContainerWindow(nsIWebBrowserChrome * *aContainerWindow) { return _to GetContainerWindow(aContainerWindow); } \
  NS_IMETHOD SetContainerWindow(nsIWebBrowserChrome * aContainerWindow) { return _to SetContainerWindow(aContainerWindow); } \
  NS_IMETHOD GetParentURIContentListener(nsIURIContentListener * *aParentURIContentListener) { return _to GetParentURIContentListener(aParentURIContentListener); } \
  NS_IMETHOD SetParentURIContentListener(nsIURIContentListener * aParentURIContentListener) { return _to SetParentURIContentListener(aParentURIContentListener); } \
  NS_IMETHOD GetContentDOMWindow(nsIDOMWindow * *aContentDOMWindow) { return _to GetContentDOMWindow(aContentDOMWindow); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBBROWSER(_to) \
  NS_IMETHOD AddWebBrowserListener(nsIWeakReference *listener, const nsIID & iid) { return !_to ? NS_ERROR_NULL_POINTER : _to->AddWebBrowserListener(listener, iid); } \
  NS_IMETHOD RemoveWebBrowserListener(nsIWeakReference *listener, const nsIID & iid) { return !_to ? NS_ERROR_NULL_POINTER : _to->RemoveWebBrowserListener(listener, iid); } \
  NS_IMETHOD GetContainerWindow(nsIWebBrowserChrome * *aContainerWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetContainerWindow(aContainerWindow); } \
  NS_IMETHOD SetContainerWindow(nsIWebBrowserChrome * aContainerWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetContainerWindow(aContainerWindow); } \
  NS_IMETHOD GetParentURIContentListener(nsIURIContentListener * *aParentURIContentListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParentURIContentListener(aParentURIContentListener); } \
  NS_IMETHOD SetParentURIContentListener(nsIURIContentListener * aParentURIContentListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetParentURIContentListener(aParentURIContentListener); } \
  NS_IMETHOD GetContentDOMWindow(nsIDOMWindow * *aContentDOMWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetContentDOMWindow(aContentDOMWindow); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebBrowser : public nsIWebBrowser
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBBROWSER

  nsWebBrowser();

private:
  ~nsWebBrowser();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebBrowser, nsIWebBrowser)

nsWebBrowser::nsWebBrowser()
{
  /* member initializers and constructor code */
}

nsWebBrowser::~nsWebBrowser()
{
  /* destructor code */
}

/* void addWebBrowserListener (in nsIWeakReference listener, in nsIIDRef iid); */
NS_IMETHODIMP nsWebBrowser::AddWebBrowserListener(nsIWeakReference *listener, const nsIID & iid)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void removeWebBrowserListener (in nsIWeakReference listener, in nsIIDRef iid); */
NS_IMETHODIMP nsWebBrowser::RemoveWebBrowserListener(nsIWeakReference *listener, const nsIID & iid)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIWebBrowserChrome containerWindow; */
NS_IMETHODIMP nsWebBrowser::GetContainerWindow(nsIWebBrowserChrome * *aContainerWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowser::SetContainerWindow(nsIWebBrowserChrome * aContainerWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIURIContentListener parentURIContentListener; */
NS_IMETHODIMP nsWebBrowser::GetParentURIContentListener(nsIURIContentListener * *aParentURIContentListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowser::SetParentURIContentListener(nsIURIContentListener * aParentURIContentListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMWindow contentDOMWindow; */
NS_IMETHODIMP nsWebBrowser::GetContentDOMWindow(nsIDOMWindow * *aContentDOMWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIWebBrowser; /* forward declaration */


/* starting interface:    nsIWebBrowserChrome */
#define NS_IWEBBROWSERCHROME_IID_STR "ba434c60-9d52-11d3-afb0-00a024ffc08c"

#define NS_IWEBBROWSERCHROME_IID \
  {0xba434c60, 0x9d52, 0x11d3, \
    { 0xaf, 0xb0, 0x00, 0xa0, 0x24, 0xff, 0xc0, 0x8c }}

class NS_NO_VTABLE nsIWebBrowserChrome : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBBROWSERCHROME_IID)

  /* void setStatus (in unsigned long status_type, in wstring status); */
  NS_IMETHOD SetStatus(PRUint32 status_type, const PRUnichar *status) = 0;

  /* attribute nsIWebBrowser webBrowser; */
  NS_IMETHOD GetWebBrowser(nsIWebBrowser * *aWebBrowser) = 0;
  NS_IMETHOD SetWebBrowser(nsIWebBrowser * aWebBrowser) = 0;

  enum { STATUS_SCRIPT = 1U };

  enum { STATUS_SCRIPT_DEFAULT = 2U };

  enum { STATUS_LINK = 3U };

  enum { CHROME_DEFAULT = 1U };

  enum { CHROME_WINDOW_BORDERS = 2U };

  enum { CHROME_WINDOW_CLOSE = 4U };

  enum { CHROME_WINDOW_RESIZE = 8U };

  enum { CHROME_MENUBAR = 16U };

  enum { CHROME_TOOLBAR = 32U };

  enum { CHROME_LOCATIONBAR = 64U };

  enum { CHROME_STATUSBAR = 128U };

  enum { CHROME_PERSONAL_TOOLBAR = 256U };

  enum { CHROME_SCROLLBARS = 512U };

  enum { CHROME_TITLEBAR = 1024U };

  enum { CHROME_WITH_SIZE = 4096U };

  enum { CHROME_WITH_POSITION = 8192U };

  enum { CHROME_WINDOW_MIN = 16384U };

  enum { CHROME_WINDOW_POPUP = 32768U };

  enum { CHROME_WINDOW_RAISED = 33554432U };

  enum { CHROME_WINDOW_LOWERED = 67108864U };

  enum { CHROME_CENTER_SCREEN = 134217728U };

  enum { CHROME_DEPENDENT = 268435456U };

  enum { CHROME_MODAL = 536870912U };

  enum { CHROME_OPENAS_DIALOG = 1073741824U };

  enum { CHROME_OPENAS_CHROME = 2147483648U };

  enum { CHROME_ALL = 4094U };

  /* attribute unsigned long chromeFlags; */
  NS_IMETHOD GetChromeFlags(PRUint32 *aChromeFlags) = 0;
  NS_IMETHOD SetChromeFlags(PRUint32 aChromeFlags) = 0;

  /* void destroyBrowserWindow (); */
  NS_IMETHOD DestroyBrowserWindow(void) = 0;

  /* void sizeBrowserTo (in long cx, in long cy); */
  NS_IMETHOD SizeBrowserTo(PRInt32 cx, PRInt32 cy) = 0;

  /* void showAsModal (); */
  NS_IMETHOD ShowAsModal(void) = 0;

  /* boolean isWindowModal (); */
  NS_IMETHOD IsWindowModal(PRBool *_retval) = 0;

  /* void exitModalEventLoop (in nsresult status); */
  NS_IMETHOD ExitModalEventLoop(nsresult status) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBBROWSERCHROME \
  NS_IMETHOD SetStatus(PRUint32 status_type, const PRUnichar *status); \
  NS_IMETHOD GetWebBrowser(nsIWebBrowser * *aWebBrowser); \
  NS_IMETHOD SetWebBrowser(nsIWebBrowser * aWebBrowser); \
  NS_IMETHOD GetChromeFlags(PRUint32 *aChromeFlags); \
  NS_IMETHOD SetChromeFlags(PRUint32 aChromeFlags); \
  NS_IMETHOD DestroyBrowserWindow(void); \
  NS_IMETHOD SizeBrowserTo(PRInt32 cx, PRInt32 cy); \
  NS_IMETHOD ShowAsModal(void); \
  NS_IMETHOD IsWindowModal(PRBool *_retval); \
  NS_IMETHOD ExitModalEventLoop(nsresult status); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBBROWSERCHROME(_to) \
  NS_IMETHOD SetStatus(PRUint32 status_type, const PRUnichar *status) { return _to SetStatus(status_type, status); } \
  NS_IMETHOD GetWebBrowser(nsIWebBrowser * *aWebBrowser) { return _to GetWebBrowser(aWebBrowser); } \
  NS_IMETHOD SetWebBrowser(nsIWebBrowser * aWebBrowser) { return _to SetWebBrowser(aWebBrowser); } \
  NS_IMETHOD GetChromeFlags(PRUint32 *aChromeFlags) { return _to GetChromeFlags(aChromeFlags); } \
  NS_IMETHOD SetChromeFlags(PRUint32 aChromeFlags) { return _to SetChromeFlags(aChromeFlags); } \
  NS_IMETHOD DestroyBrowserWindow(void) { return _to DestroyBrowserWindow(); } \
  NS_IMETHOD SizeBrowserTo(PRInt32 cx, PRInt32 cy) { return _to SizeBrowserTo(cx, cy); } \
  NS_IMETHOD ShowAsModal(void) { return _to ShowAsModal(); } \
  NS_IMETHOD IsWindowModal(PRBool *_retval) { return _to IsWindowModal(_retval); } \
  NS_IMETHOD ExitModalEventLoop(nsresult status) { return _to ExitModalEventLoop(status); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBBROWSERCHROME(_to) \
  NS_IMETHOD SetStatus(PRUint32 status_type, const PRUnichar *status) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetStatus(status_type, status); } \
  NS_IMETHOD GetWebBrowser(nsIWebBrowser * *aWebBrowser) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWebBrowser(aWebBrowser); } \
  NS_IMETHOD SetWebBrowser(nsIWebBrowser * aWebBrowser) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetWebBrowser(aWebBrowser); } \
  NS_IMETHOD GetChromeFlags(PRUint32 *aChromeFlags) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChromeFlags(aChromeFlags); } \
  NS_IMETHOD SetChromeFlags(PRUint32 aChromeFlags) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetChromeFlags(aChromeFlags); } \
  NS_IMETHOD DestroyBrowserWindow(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->DestroyBrowserWindow(); } \
  NS_IMETHOD SizeBrowserTo(PRInt32 cx, PRInt32 cy) { return !_to ? NS_ERROR_NULL_POINTER : _to->SizeBrowserTo(cx, cy); } \
  NS_IMETHOD ShowAsModal(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ShowAsModal(); } \
  NS_IMETHOD IsWindowModal(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsWindowModal(_retval); } \
  NS_IMETHOD ExitModalEventLoop(nsresult status) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExitModalEventLoop(status); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebBrowserChrome : public nsIWebBrowserChrome
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBBROWSERCHROME

  nsWebBrowserChrome();

private:
  ~nsWebBrowserChrome();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebBrowserChrome, nsIWebBrowserChrome)

nsWebBrowserChrome::nsWebBrowserChrome()
{
  /* member initializers and constructor code */
}

nsWebBrowserChrome::~nsWebBrowserChrome()
{
  /* destructor code */
}

/* void setStatus (in unsigned long status_type, in wstring status); */
NS_IMETHODIMP nsWebBrowserChrome::SetStatus(PRUint32 status_type, const PRUnichar *status)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIWebBrowser webBrowser; */
NS_IMETHODIMP nsWebBrowserChrome::GetWebBrowser(nsIWebBrowser * *aWebBrowser)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserChrome::SetWebBrowser(nsIWebBrowser * aWebBrowser)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute unsigned long chromeFlags; */
NS_IMETHODIMP nsWebBrowserChrome::GetChromeFlags(PRUint32 *aChromeFlags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserChrome::SetChromeFlags(PRUint32 aChromeFlags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void destroyBrowserWindow (); */
NS_IMETHODIMP nsWebBrowserChrome::DestroyBrowserWindow()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void sizeBrowserTo (in long cx, in long cy); */
NS_IMETHODIMP nsWebBrowserChrome::SizeBrowserTo(PRInt32 cx, PRInt32 cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void showAsModal (); */
NS_IMETHODIMP nsWebBrowserChrome::ShowAsModal()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isWindowModal (); */
NS_IMETHODIMP nsWebBrowserChrome::IsWindowModal(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void exitModalEventLoop (in nsresult status); */
NS_IMETHODIMP nsWebBrowserChrome::ExitModalEventLoop(nsresult status)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIWebBrowserChromeFocus */
#define NS_IWEBBROWSERCHROMEFOCUS_IID_STR "d2206418-1dd1-11b2-8e55-acddcd2bcfb8"

#define NS_IWEBBROWSERCHROMEFOCUS_IID \
  {0xd2206418, 0x1dd1, 0x11b2, \
    { 0x8e, 0x55, 0xac, 0xdd, 0xcd, 0x2b, 0xcf, 0xb8 }}

class NS_NO_VTABLE nsIWebBrowserChromeFocus : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBBROWSERCHROMEFOCUS_IID)

  /* void focusNextElement (); */
  NS_IMETHOD FocusNextElement(void) = 0;

  /* void focusPrevElement (); */
  NS_IMETHOD FocusPrevElement(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBBROWSERCHROMEFOCUS \
  NS_IMETHOD FocusNextElement(void); \
  NS_IMETHOD FocusPrevElement(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBBROWSERCHROMEFOCUS(_to) \
  NS_IMETHOD FocusNextElement(void) { return _to FocusNextElement(); } \
  NS_IMETHOD FocusPrevElement(void) { return _to FocusPrevElement(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBBROWSERCHROMEFOCUS(_to) \
  NS_IMETHOD FocusNextElement(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->FocusNextElement(); } \
  NS_IMETHOD FocusPrevElement(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->FocusPrevElement(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebBrowserChromeFocus : public nsIWebBrowserChromeFocus
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBBROWSERCHROMEFOCUS

  nsWebBrowserChromeFocus();

private:
  ~nsWebBrowserChromeFocus();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebBrowserChromeFocus, nsIWebBrowserChromeFocus)

nsWebBrowserChromeFocus::nsWebBrowserChromeFocus()
{
  /* member initializers and constructor code */
}

nsWebBrowserChromeFocus::~nsWebBrowserChromeFocus()
{
  /* destructor code */
}

/* void focusNextElement (); */
NS_IMETHODIMP nsWebBrowserChromeFocus::FocusNextElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void focusPrevElement (); */
NS_IMETHODIMP nsWebBrowserChromeFocus::FocusPrevElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIWebBrowserFind */
#define NS_IWEBBROWSERFIND_IID_STR "2f977d44-5485-11d4-87e2-0010a4e75ef2"

#define NS_IWEBBROWSERFIND_IID \
  {0x2f977d44, 0x5485, 0x11d4, \
    { 0x87, 0xe2, 0x00, 0x10, 0xa4, 0xe7, 0x5e, 0xf2 }}

class NS_NO_VTABLE nsIWebBrowserFind : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBBROWSERFIND_IID)

  /* boolean findNext (); */
  NS_IMETHOD FindNext(PRBool *_retval) = 0;

  /* attribute wstring searchString; */
  NS_IMETHOD GetSearchString(PRUnichar * *aSearchString) = 0;
  NS_IMETHOD SetSearchString(const PRUnichar * aSearchString) = 0;

  /* attribute boolean findBackwards; */
  NS_IMETHOD GetFindBackwards(PRBool *aFindBackwards) = 0;
  NS_IMETHOD SetFindBackwards(PRBool aFindBackwards) = 0;

  /* attribute boolean wrapFind; */
  NS_IMETHOD GetWrapFind(PRBool *aWrapFind) = 0;
  NS_IMETHOD SetWrapFind(PRBool aWrapFind) = 0;

  /* attribute boolean entireWord; */
  NS_IMETHOD GetEntireWord(PRBool *aEntireWord) = 0;
  NS_IMETHOD SetEntireWord(PRBool aEntireWord) = 0;

  /* attribute boolean matchCase; */
  NS_IMETHOD GetMatchCase(PRBool *aMatchCase) = 0;
  NS_IMETHOD SetMatchCase(PRBool aMatchCase) = 0;

  /* attribute boolean searchFrames; */
  NS_IMETHOD GetSearchFrames(PRBool *aSearchFrames) = 0;
  NS_IMETHOD SetSearchFrames(PRBool aSearchFrames) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBBROWSERFIND \
  NS_IMETHOD FindNext(PRBool *_retval); \
  NS_IMETHOD GetSearchString(PRUnichar * *aSearchString); \
  NS_IMETHOD SetSearchString(const PRUnichar * aSearchString); \
  NS_IMETHOD GetFindBackwards(PRBool *aFindBackwards); \
  NS_IMETHOD SetFindBackwards(PRBool aFindBackwards); \
  NS_IMETHOD GetWrapFind(PRBool *aWrapFind); \
  NS_IMETHOD SetWrapFind(PRBool aWrapFind); \
  NS_IMETHOD GetEntireWord(PRBool *aEntireWord); \
  NS_IMETHOD SetEntireWord(PRBool aEntireWord); \
  NS_IMETHOD GetMatchCase(PRBool *aMatchCase); \
  NS_IMETHOD SetMatchCase(PRBool aMatchCase); \
  NS_IMETHOD GetSearchFrames(PRBool *aSearchFrames); \
  NS_IMETHOD SetSearchFrames(PRBool aSearchFrames); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBBROWSERFIND(_to) \
  NS_IMETHOD FindNext(PRBool *_retval) { return _to FindNext(_retval); } \
  NS_IMETHOD GetSearchString(PRUnichar * *aSearchString) { return _to GetSearchString(aSearchString); } \
  NS_IMETHOD SetSearchString(const PRUnichar * aSearchString) { return _to SetSearchString(aSearchString); } \
  NS_IMETHOD GetFindBackwards(PRBool *aFindBackwards) { return _to GetFindBackwards(aFindBackwards); } \
  NS_IMETHOD SetFindBackwards(PRBool aFindBackwards) { return _to SetFindBackwards(aFindBackwards); } \
  NS_IMETHOD GetWrapFind(PRBool *aWrapFind) { return _to GetWrapFind(aWrapFind); } \
  NS_IMETHOD SetWrapFind(PRBool aWrapFind) { return _to SetWrapFind(aWrapFind); } \
  NS_IMETHOD GetEntireWord(PRBool *aEntireWord) { return _to GetEntireWord(aEntireWord); } \
  NS_IMETHOD SetEntireWord(PRBool aEntireWord) { return _to SetEntireWord(aEntireWord); } \
  NS_IMETHOD GetMatchCase(PRBool *aMatchCase) { return _to GetMatchCase(aMatchCase); } \
  NS_IMETHOD SetMatchCase(PRBool aMatchCase) { return _to SetMatchCase(aMatchCase); } \
  NS_IMETHOD GetSearchFrames(PRBool *aSearchFrames) { return _to GetSearchFrames(aSearchFrames); } \
  NS_IMETHOD SetSearchFrames(PRBool aSearchFrames) { return _to SetSearchFrames(aSearchFrames); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBBROWSERFIND(_to) \
  NS_IMETHOD FindNext(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->FindNext(_retval); } \
  NS_IMETHOD GetSearchString(PRUnichar * *aSearchString) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSearchString(aSearchString); } \
  NS_IMETHOD SetSearchString(const PRUnichar * aSearchString) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSearchString(aSearchString); } \
  NS_IMETHOD GetFindBackwards(PRBool *aFindBackwards) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFindBackwards(aFindBackwards); } \
  NS_IMETHOD SetFindBackwards(PRBool aFindBackwards) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFindBackwards(aFindBackwards); } \
  NS_IMETHOD GetWrapFind(PRBool *aWrapFind) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWrapFind(aWrapFind); } \
  NS_IMETHOD SetWrapFind(PRBool aWrapFind) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetWrapFind(aWrapFind); } \
  NS_IMETHOD GetEntireWord(PRBool *aEntireWord) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetEntireWord(aEntireWord); } \
  NS_IMETHOD SetEntireWord(PRBool aEntireWord) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetEntireWord(aEntireWord); } \
  NS_IMETHOD GetMatchCase(PRBool *aMatchCase) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMatchCase(aMatchCase); } \
  NS_IMETHOD SetMatchCase(PRBool aMatchCase) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetMatchCase(aMatchCase); } \
  NS_IMETHOD GetSearchFrames(PRBool *aSearchFrames) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSearchFrames(aSearchFrames); } \
  NS_IMETHOD SetSearchFrames(PRBool aSearchFrames) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSearchFrames(aSearchFrames); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebBrowserFind : public nsIWebBrowserFind
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBBROWSERFIND

  nsWebBrowserFind();

private:
  ~nsWebBrowserFind();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebBrowserFind, nsIWebBrowserFind)

nsWebBrowserFind::nsWebBrowserFind()
{
  /* member initializers and constructor code */
}

nsWebBrowserFind::~nsWebBrowserFind()
{
  /* destructor code */
}

/* boolean findNext (); */
NS_IMETHODIMP nsWebBrowserFind::FindNext(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring searchString; */
NS_IMETHODIMP nsWebBrowserFind::GetSearchString(PRUnichar * *aSearchString)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFind::SetSearchString(const PRUnichar * aSearchString)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean findBackwards; */
NS_IMETHODIMP nsWebBrowserFind::GetFindBackwards(PRBool *aFindBackwards)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFind::SetFindBackwards(PRBool aFindBackwards)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean wrapFind; */
NS_IMETHODIMP nsWebBrowserFind::GetWrapFind(PRBool *aWrapFind)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFind::SetWrapFind(PRBool aWrapFind)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean entireWord; */
NS_IMETHODIMP nsWebBrowserFind::GetEntireWord(PRBool *aEntireWord)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFind::SetEntireWord(PRBool aEntireWord)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean matchCase; */
NS_IMETHODIMP nsWebBrowserFind::GetMatchCase(PRBool *aMatchCase)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFind::SetMatchCase(PRBool aMatchCase)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean searchFrames; */
NS_IMETHODIMP nsWebBrowserFind::GetSearchFrames(PRBool *aSearchFrames)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFind::SetSearchFrames(PRBool aSearchFrames)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMWindow; /* forward declaration */

class nsIDOMElement; /* forward declaration */


/* starting interface:    nsIWebBrowserFocus */
#define NS_IWEBBROWSERFOCUS_IID_STR "9c5d3c58-1dd1-11b2-a1c9-f3699284657a"

#define NS_IWEBBROWSERFOCUS_IID \
  {0x9c5d3c58, 0x1dd1, 0x11b2, \
    { 0xa1, 0xc9, 0xf3, 0x69, 0x92, 0x84, 0x65, 0x7a }}

class NS_NO_VTABLE nsIWebBrowserFocus : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBBROWSERFOCUS_IID)

  /* void activate (); */
  NS_IMETHOD Activate(void) = 0;

  /* void deactivate (); */
  NS_IMETHOD Deactivate(void) = 0;

  /* void setFocusAtFirstElement (); */
  NS_IMETHOD SetFocusAtFirstElement(void) = 0;

  /* void setFocusAtLastElement (); */
  NS_IMETHOD SetFocusAtLastElement(void) = 0;

  /* attribute nsIDOMWindow focusedWindow; */
  NS_IMETHOD GetFocusedWindow(nsIDOMWindow * *aFocusedWindow) = 0;
  NS_IMETHOD SetFocusedWindow(nsIDOMWindow * aFocusedWindow) = 0;

  /* attribute nsIDOMElement focusedElement; */
  NS_IMETHOD GetFocusedElement(nsIDOMElement * *aFocusedElement) = 0;
  NS_IMETHOD SetFocusedElement(nsIDOMElement * aFocusedElement) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBBROWSERFOCUS \
  NS_IMETHOD Activate(void); \
  NS_IMETHOD Deactivate(void); \
  NS_IMETHOD SetFocusAtFirstElement(void); \
  NS_IMETHOD SetFocusAtLastElement(void); \
  NS_IMETHOD GetFocusedWindow(nsIDOMWindow * *aFocusedWindow); \
  NS_IMETHOD SetFocusedWindow(nsIDOMWindow * aFocusedWindow); \
  NS_IMETHOD GetFocusedElement(nsIDOMElement * *aFocusedElement); \
  NS_IMETHOD SetFocusedElement(nsIDOMElement * aFocusedElement); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBBROWSERFOCUS(_to) \
  NS_IMETHOD Activate(void) { return _to Activate(); } \
  NS_IMETHOD Deactivate(void) { return _to Deactivate(); } \
  NS_IMETHOD SetFocusAtFirstElement(void) { return _to SetFocusAtFirstElement(); } \
  NS_IMETHOD SetFocusAtLastElement(void) { return _to SetFocusAtLastElement(); } \
  NS_IMETHOD GetFocusedWindow(nsIDOMWindow * *aFocusedWindow) { return _to GetFocusedWindow(aFocusedWindow); } \
  NS_IMETHOD SetFocusedWindow(nsIDOMWindow * aFocusedWindow) { return _to SetFocusedWindow(aFocusedWindow); } \
  NS_IMETHOD GetFocusedElement(nsIDOMElement * *aFocusedElement) { return _to GetFocusedElement(aFocusedElement); } \
  NS_IMETHOD SetFocusedElement(nsIDOMElement * aFocusedElement) { return _to SetFocusedElement(aFocusedElement); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBBROWSERFOCUS(_to) \
  NS_IMETHOD Activate(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Activate(); } \
  NS_IMETHOD Deactivate(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Deactivate(); } \
  NS_IMETHOD SetFocusAtFirstElement(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFocusAtFirstElement(); } \
  NS_IMETHOD SetFocusAtLastElement(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFocusAtLastElement(); } \
  NS_IMETHOD GetFocusedWindow(nsIDOMWindow * *aFocusedWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFocusedWindow(aFocusedWindow); } \
  NS_IMETHOD SetFocusedWindow(nsIDOMWindow * aFocusedWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFocusedWindow(aFocusedWindow); } \
  NS_IMETHOD GetFocusedElement(nsIDOMElement * *aFocusedElement) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFocusedElement(aFocusedElement); } \
  NS_IMETHOD SetFocusedElement(nsIDOMElement * aFocusedElement) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFocusedElement(aFocusedElement); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebBrowserFocus : public nsIWebBrowserFocus
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBBROWSERFOCUS

  nsWebBrowserFocus();

private:
  ~nsWebBrowserFocus();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebBrowserFocus, nsIWebBrowserFocus)

nsWebBrowserFocus::nsWebBrowserFocus()
{
  /* member initializers and constructor code */
}

nsWebBrowserFocus::~nsWebBrowserFocus()
{
  /* destructor code */
}

/* void activate (); */
NS_IMETHODIMP nsWebBrowserFocus::Activate()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void deactivate (); */
NS_IMETHODIMP nsWebBrowserFocus::Deactivate()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setFocusAtFirstElement (); */
NS_IMETHODIMP nsWebBrowserFocus::SetFocusAtFirstElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setFocusAtLastElement (); */
NS_IMETHODIMP nsWebBrowserFocus::SetFocusAtLastElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIDOMWindow focusedWindow; */
NS_IMETHODIMP nsWebBrowserFocus::GetFocusedWindow(nsIDOMWindow * *aFocusedWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFocus::SetFocusedWindow(nsIDOMWindow * aFocusedWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIDOMElement focusedElement; */
NS_IMETHODIMP nsWebBrowserFocus::GetFocusedElement(nsIDOMElement * *aFocusedElement)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserFocus::SetFocusedElement(nsIDOMElement * aFocusedElement)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIURI; /* forward declaration */

class nsIFile; /* forward declaration */

class nsIChannel; /* forward declaration */

class nsIWebProgressListener; /* forward declaration */

class nsIInputStream; /* forward declaration */

class nsIDOMDocument; /* forward declaration */


/* starting interface:    nsIWebBrowserPersist */
#define NS_IWEBBROWSERPERSIST_IID_STR "dd4e0a6a-210f-419a-ad85-40e8543b9465"

#define NS_IWEBBROWSERPERSIST_IID \
  {0xdd4e0a6a, 0x210f, 0x419a, \
    { 0xad, 0x85, 0x40, 0xe8, 0x54, 0x3b, 0x94, 0x65 }}

class NS_NO_VTABLE nsIWebBrowserPersist : public nsICancelable {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBBROWSERPERSIST_IID)

  enum { PERSIST_FLAGS_NONE = 0U };

  enum { PERSIST_FLAGS_FROM_CACHE = 1U };

  enum { PERSIST_FLAGS_BYPASS_CACHE = 2U };

  enum { PERSIST_FLAGS_IGNORE_REDIRECTED_DATA = 4U };

  enum { PERSIST_FLAGS_IGNORE_IFRAMES = 8U };

  enum { PERSIST_FLAGS_NO_CONVERSION = 16U };

  enum { PERSIST_FLAGS_REPLACE_EXISTING_FILES = 32U };

  enum { PERSIST_FLAGS_NO_BASE_TAG_MODIFICATIONS = 64U };

  enum { PERSIST_FLAGS_FIXUP_ORIGINAL_DOM = 128U };

  enum { PERSIST_FLAGS_FIXUP_LINKS_TO_DESTINATION = 256U };

  enum { PERSIST_FLAGS_DONT_FIXUP_LINKS = 512U };

  enum { PERSIST_FLAGS_SERIALIZE_OUTPUT = 1024U };

  enum { PERSIST_FLAGS_DONT_CHANGE_FILENAMES = 2048U };

  enum { PERSIST_FLAGS_FAIL_ON_BROKEN_LINKS = 4096U };

  enum { PERSIST_FLAGS_CLEANUP_ON_FAILURE = 8192U };

  enum { PERSIST_FLAGS_AUTODETECT_APPLY_CONVERSION = 16384U };

  enum { PERSIST_STATE_READY = 1U };

  enum { PERSIST_STATE_SAVING = 2U };

  enum { PERSIST_STATE_FINISHED = 3U };

  enum { ENCODE_FLAGS_SELECTION_ONLY = 1U };

  enum { ENCODE_FLAGS_FORMATTED = 2U };

  enum { ENCODE_FLAGS_RAW = 4U };

  enum { ENCODE_FLAGS_BODY_ONLY = 8U };

  enum { ENCODE_FLAGS_PREFORMATTED = 16U };

  enum { ENCODE_FLAGS_WRAP = 32U };

  enum { ENCODE_FLAGS_FORMAT_FLOWED = 64U };

  enum { ENCODE_FLAGS_ABSOLUTE_LINKS = 128U };

  enum { ENCODE_FLAGS_ENCODE_W3C_ENTITIES = 256U };

  enum { ENCODE_FLAGS_CR_LINEBREAKS = 512U };

  enum { ENCODE_FLAGS_LF_LINEBREAKS = 1024U };

  enum { ENCODE_FLAGS_NOSCRIPT_CONTENT = 2048U };

  enum { ENCODE_FLAGS_NOFRAMES_CONTENT = 4096U };

  enum { ENCODE_FLAGS_ENCODE_BASIC_ENTITIES = 8192U };

  enum { ENCODE_FLAGS_ENCODE_LATIN1_ENTITIES = 16384U };

  enum { ENCODE_FLAGS_ENCODE_HTML_ENTITIES = 32768U };

  /* attribute unsigned long persistFlags; */
  NS_IMETHOD GetPersistFlags(PRUint32 *aPersistFlags) = 0;
  NS_IMETHOD SetPersistFlags(PRUint32 aPersistFlags) = 0;

  /* readonly attribute unsigned long currentState; */
  NS_IMETHOD GetCurrentState(PRUint32 *aCurrentState) = 0;

  /* readonly attribute unsigned long result; */
  NS_IMETHOD GetResult(PRUint32 *aResult) = 0;

  /* attribute nsIWebProgressListener progressListener; */
  NS_IMETHOD GetProgressListener(nsIWebProgressListener * *aProgressListener) = 0;
  NS_IMETHOD SetProgressListener(nsIWebProgressListener * aProgressListener) = 0;

  /* void saveURI (in nsIURI uri, in nsISupports cache_key, in nsIURI referrer, in nsIInputStream post_data, in string extra_headers, in nsIFile file); */
  NS_IMETHOD SaveURI(nsIURI *uri, nsISupports *cache_key, nsIURI *referrer, nsIInputStream *post_data, const char *extra_headers, nsIFile *file) = 0;

  /* void saveChannel (in nsIChannel channel, in nsISupports file); */
  NS_IMETHOD SaveChannel(nsIChannel *channel, nsISupports *file) = 0;

  /* void saveDocument (in nsIDOMDocument document, in nsISupports file, in nsISupports data_path, in string output_content_type, in unsigned long encoding_flags, in unsigned long wrap_column); */
  NS_IMETHOD SaveDocument(nsIDOMDocument *document, nsISupports *file, nsISupports *data_path, const char *output_content_type, PRUint32 encoding_flags, PRUint32 wrap_column) = 0;

  /* void cancelSave (); */
  NS_IMETHOD CancelSave(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBBROWSERPERSIST \
  NS_IMETHOD GetPersistFlags(PRUint32 *aPersistFlags); \
  NS_IMETHOD SetPersistFlags(PRUint32 aPersistFlags); \
  NS_IMETHOD GetCurrentState(PRUint32 *aCurrentState); \
  NS_IMETHOD GetResult(PRUint32 *aResult); \
  NS_IMETHOD GetProgressListener(nsIWebProgressListener * *aProgressListener); \
  NS_IMETHOD SetProgressListener(nsIWebProgressListener * aProgressListener); \
  NS_IMETHOD SaveURI(nsIURI *uri, nsISupports *cache_key, nsIURI *referrer, nsIInputStream *post_data, const char *extra_headers, nsIFile *file); \
  NS_IMETHOD SaveChannel(nsIChannel *channel, nsISupports *file); \
  NS_IMETHOD SaveDocument(nsIDOMDocument *document, nsISupports *file, nsISupports *data_path, const char *output_content_type, PRUint32 encoding_flags, PRUint32 wrap_column); \
  NS_IMETHOD CancelSave(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBBROWSERPERSIST(_to) \
  NS_IMETHOD GetPersistFlags(PRUint32 *aPersistFlags) { return _to GetPersistFlags(aPersistFlags); } \
  NS_IMETHOD SetPersistFlags(PRUint32 aPersistFlags) { return _to SetPersistFlags(aPersistFlags); } \
  NS_IMETHOD GetCurrentState(PRUint32 *aCurrentState) { return _to GetCurrentState(aCurrentState); } \
  NS_IMETHOD GetResult(PRUint32 *aResult) { return _to GetResult(aResult); } \
  NS_IMETHOD GetProgressListener(nsIWebProgressListener * *aProgressListener) { return _to GetProgressListener(aProgressListener); } \
  NS_IMETHOD SetProgressListener(nsIWebProgressListener * aProgressListener) { return _to SetProgressListener(aProgressListener); } \
  NS_IMETHOD SaveURI(nsIURI *uri, nsISupports *cache_key, nsIURI *referrer, nsIInputStream *post_data, const char *extra_headers, nsIFile *file) { return _to SaveURI(uri, cache_key, referrer, post_data, extra_headers, file); } \
  NS_IMETHOD SaveChannel(nsIChannel *channel, nsISupports *file) { return _to SaveChannel(channel, file); } \
  NS_IMETHOD SaveDocument(nsIDOMDocument *document, nsISupports *file, nsISupports *data_path, const char *output_content_type, PRUint32 encoding_flags, PRUint32 wrap_column) { return _to SaveDocument(document, file, data_path, output_content_type, encoding_flags, wrap_column); } \
  NS_IMETHOD CancelSave(void) { return _to CancelSave(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBBROWSERPERSIST(_to) \
  NS_IMETHOD GetPersistFlags(PRUint32 *aPersistFlags) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPersistFlags(aPersistFlags); } \
  NS_IMETHOD SetPersistFlags(PRUint32 aPersistFlags) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetPersistFlags(aPersistFlags); } \
  NS_IMETHOD GetCurrentState(PRUint32 *aCurrentState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCurrentState(aCurrentState); } \
  NS_IMETHOD GetResult(PRUint32 *aResult) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetResult(aResult); } \
  NS_IMETHOD GetProgressListener(nsIWebProgressListener * *aProgressListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetProgressListener(aProgressListener); } \
  NS_IMETHOD SetProgressListener(nsIWebProgressListener * aProgressListener) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetProgressListener(aProgressListener); } \
  NS_IMETHOD SaveURI(nsIURI *uri, nsISupports *cache_key, nsIURI *referrer, nsIInputStream *post_data, const char *extra_headers, nsIFile *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->SaveURI(uri, cache_key, referrer, post_data, extra_headers, file); } \
  NS_IMETHOD SaveChannel(nsIChannel *channel, nsISupports *file) { return !_to ? NS_ERROR_NULL_POINTER : _to->SaveChannel(channel, file); } \
  NS_IMETHOD SaveDocument(nsIDOMDocument *document, nsISupports *file, nsISupports *data_path, const char *output_content_type, PRUint32 encoding_flags, PRUint32 wrap_column) { return !_to ? NS_ERROR_NULL_POINTER : _to->SaveDocument(document, file, data_path, output_content_type, encoding_flags, wrap_column); } \
  NS_IMETHOD CancelSave(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CancelSave(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebBrowserPersist : public nsIWebBrowserPersist
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBBROWSERPERSIST

  nsWebBrowserPersist();

private:
  ~nsWebBrowserPersist();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebBrowserPersist, nsIWebBrowserPersist)

nsWebBrowserPersist::nsWebBrowserPersist()
{
  /* member initializers and constructor code */
}

nsWebBrowserPersist::~nsWebBrowserPersist()
{
  /* destructor code */
}

/* attribute unsigned long persistFlags; */
NS_IMETHODIMP nsWebBrowserPersist::GetPersistFlags(PRUint32 *aPersistFlags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserPersist::SetPersistFlags(PRUint32 aPersistFlags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned long currentState; */
NS_IMETHODIMP nsWebBrowserPersist::GetCurrentState(PRUint32 *aCurrentState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned long result; */
NS_IMETHODIMP nsWebBrowserPersist::GetResult(PRUint32 *aResult)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIWebProgressListener progressListener; */
NS_IMETHODIMP nsWebBrowserPersist::GetProgressListener(nsIWebProgressListener * *aProgressListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebBrowserPersist::SetProgressListener(nsIWebProgressListener * aProgressListener)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void saveURI (in nsIURI uri, in nsISupports cache_key, in nsIURI referrer, in nsIInputStream post_data, in string extra_headers, in nsIFile file); */
NS_IMETHODIMP nsWebBrowserPersist::SaveURI(nsIURI *uri, nsISupports *cache_key, nsIURI *referrer, nsIInputStream *post_data, const char *extra_headers, nsIFile *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void saveChannel (in nsIChannel channel, in nsISupports file); */
NS_IMETHODIMP nsWebBrowserPersist::SaveChannel(nsIChannel *channel, nsISupports *file)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void saveDocument (in nsIDOMDocument document, in nsISupports file, in nsISupports data_path, in string output_content_type, in unsigned long encoding_flags, in unsigned long wrap_column); */
NS_IMETHODIMP nsWebBrowserPersist::SaveDocument(nsIDOMDocument *document, nsISupports *file, nsISupports *data_path, const char *output_content_type, PRUint32 encoding_flags, PRUint32 wrap_column)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void cancelSave (); */
NS_IMETHODIMP nsWebBrowserPersist::CancelSave()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIURI; /* forward declaration */

class nsIInputStream; /* forward declaration */

class nsIDOMDocument; /* forward declaration */

class nsISHistory; /* forward declaration */


/* starting interface:    nsIWebNavigation */
#define NS_IWEBNAVIGATION_IID_STR "f5d9e7b0-d930-11d3-b057-00a024ffc08c"

#define NS_IWEBNAVIGATION_IID \
  {0xf5d9e7b0, 0xd930, 0x11d3, \
    { 0xb0, 0x57, 0x00, 0xa0, 0x24, 0xff, 0xc0, 0x8c }}

class NS_NO_VTABLE nsIWebNavigation : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWEBNAVIGATION_IID)

  /* readonly attribute boolean canGoBack; */
  NS_IMETHOD GetCanGoBack(PRBool *aCanGoBack) = 0;

  /* readonly attribute boolean canGoForward; */
  NS_IMETHOD GetCanGoForward(PRBool *aCanGoForward) = 0;

  /* void goBack (); */
  NS_IMETHOD GoBack(void) = 0;

  /* void goForward (); */
  NS_IMETHOD GoForward(void) = 0;

  /* void gotoIndex (in long index); */
  NS_IMETHOD GotoIndex(PRInt32 index) = 0;

  enum { LOAD_FLAGS_MASK = 65535U };

  enum { LOAD_FLAGS_NONE = 0U };

  enum { LOAD_FLAGS_IS_REFRESH = 16U };

  enum { LOAD_FLAGS_IS_LINK = 32U };

  enum { LOAD_FLAGS_BYPASS_HISTORY = 64U };

  enum { LOAD_FLAGS_REPLACE_HISTORY = 128U };

  enum { LOAD_FLAGS_BYPASS_CACHE = 256U };

  enum { LOAD_FLAGS_BYPASS_PROXY = 512U };

  enum { LOAD_FLAGS_CHARSET_CHANGE = 1024U };

  enum { LOAD_FLAGS_STOP_CONTENT = 2048U };

  enum { LOAD_FLAGS_FROM_EXTERNAL = 4096U };

  enum { STOP_NETWORK = 1U };

  enum { STOP_CONTENT = 2U };

  enum { STOP_ALL = 3U };

  /* void loadURI (in wstring uri, in unsigned long load_flags, in nsIURI referrer, in nsIInputStream post_data, in nsIInputStream headers); */
  NS_IMETHOD LoadURI(const PRUnichar *uri, PRUint32 load_flags, nsIURI *referrer, nsIInputStream *post_data, nsIInputStream *headers) = 0;

  /* void reload (in unsigned long reload_flags); */
  NS_IMETHOD Reload(PRUint32 reload_flags) = 0;

  /* void stop (in unsigned long stop_flags); */
  NS_IMETHOD Stop(PRUint32 stop_flags) = 0;

  /* readonly attribute nsIDOMDocument document; */
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument) = 0;

  /* readonly attribute nsIURI currentURI; */
  NS_IMETHOD GetCurrentURI(nsIURI * *aCurrentURI) = 0;

  /* readonly attribute nsIURI referringURI; */
  NS_IMETHOD GetReferringURI(nsIURI * *aReferringURI) = 0;

  /* attribute nsISHistory sessionHistory; */
  NS_IMETHOD GetSessionHistory(nsISHistory * *aSessionHistory) = 0;
  NS_IMETHOD SetSessionHistory(nsISHistory * aSessionHistory) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWEBNAVIGATION \
  NS_IMETHOD GetCanGoBack(PRBool *aCanGoBack); \
  NS_IMETHOD GetCanGoForward(PRBool *aCanGoForward); \
  NS_IMETHOD GoBack(void); \
  NS_IMETHOD GoForward(void); \
  NS_IMETHOD GotoIndex(PRInt32 index); \
  NS_IMETHOD LoadURI(const PRUnichar *uri, PRUint32 load_flags, nsIURI *referrer, nsIInputStream *post_data, nsIInputStream *headers); \
  NS_IMETHOD Reload(PRUint32 reload_flags); \
  NS_IMETHOD Stop(PRUint32 stop_flags); \
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument); \
  NS_IMETHOD GetCurrentURI(nsIURI * *aCurrentURI); \
  NS_IMETHOD GetReferringURI(nsIURI * *aReferringURI); \
  NS_IMETHOD GetSessionHistory(nsISHistory * *aSessionHistory); \
  NS_IMETHOD SetSessionHistory(nsISHistory * aSessionHistory); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWEBNAVIGATION(_to) \
  NS_IMETHOD GetCanGoBack(PRBool *aCanGoBack) { return _to GetCanGoBack(aCanGoBack); } \
  NS_IMETHOD GetCanGoForward(PRBool *aCanGoForward) { return _to GetCanGoForward(aCanGoForward); } \
  NS_IMETHOD GoBack(void) { return _to GoBack(); } \
  NS_IMETHOD GoForward(void) { return _to GoForward(); } \
  NS_IMETHOD GotoIndex(PRInt32 index) { return _to GotoIndex(index); } \
  NS_IMETHOD LoadURI(const PRUnichar *uri, PRUint32 load_flags, nsIURI *referrer, nsIInputStream *post_data, nsIInputStream *headers) { return _to LoadURI(uri, load_flags, referrer, post_data, headers); } \
  NS_IMETHOD Reload(PRUint32 reload_flags) { return _to Reload(reload_flags); } \
  NS_IMETHOD Stop(PRUint32 stop_flags) { return _to Stop(stop_flags); } \
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument) { return _to GetDocument(aDocument); } \
  NS_IMETHOD GetCurrentURI(nsIURI * *aCurrentURI) { return _to GetCurrentURI(aCurrentURI); } \
  NS_IMETHOD GetReferringURI(nsIURI * *aReferringURI) { return _to GetReferringURI(aReferringURI); } \
  NS_IMETHOD GetSessionHistory(nsISHistory * *aSessionHistory) { return _to GetSessionHistory(aSessionHistory); } \
  NS_IMETHOD SetSessionHistory(nsISHistory * aSessionHistory) { return _to SetSessionHistory(aSessionHistory); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWEBNAVIGATION(_to) \
  NS_IMETHOD GetCanGoBack(PRBool *aCanGoBack) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCanGoBack(aCanGoBack); } \
  NS_IMETHOD GetCanGoForward(PRBool *aCanGoForward) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCanGoForward(aCanGoForward); } \
  NS_IMETHOD GoBack(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->GoBack(); } \
  NS_IMETHOD GoForward(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->GoForward(); } \
  NS_IMETHOD GotoIndex(PRInt32 index) { return !_to ? NS_ERROR_NULL_POINTER : _to->GotoIndex(index); } \
  NS_IMETHOD LoadURI(const PRUnichar *uri, PRUint32 load_flags, nsIURI *referrer, nsIInputStream *post_data, nsIInputStream *headers) { return !_to ? NS_ERROR_NULL_POINTER : _to->LoadURI(uri, load_flags, referrer, post_data, headers); } \
  NS_IMETHOD Reload(PRUint32 reload_flags) { return !_to ? NS_ERROR_NULL_POINTER : _to->Reload(reload_flags); } \
  NS_IMETHOD Stop(PRUint32 stop_flags) { return !_to ? NS_ERROR_NULL_POINTER : _to->Stop(stop_flags); } \
  NS_IMETHOD GetDocument(nsIDOMDocument * *aDocument) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDocument(aDocument); } \
  NS_IMETHOD GetCurrentURI(nsIURI * *aCurrentURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCurrentURI(aCurrentURI); } \
  NS_IMETHOD GetReferringURI(nsIURI * *aReferringURI) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetReferringURI(aReferringURI); } \
  NS_IMETHOD GetSessionHistory(nsISHistory * *aSessionHistory) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSessionHistory(aSessionHistory); } \
  NS_IMETHOD SetSessionHistory(nsISHistory * aSessionHistory) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSessionHistory(aSessionHistory); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWebNavigation : public nsIWebNavigation
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBNAVIGATION

  nsWebNavigation();

private:
  ~nsWebNavigation();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWebNavigation, nsIWebNavigation)

nsWebNavigation::nsWebNavigation()
{
  /* member initializers and constructor code */
}

nsWebNavigation::~nsWebNavigation()
{
  /* destructor code */
}

/* readonly attribute boolean canGoBack; */
NS_IMETHODIMP nsWebNavigation::GetCanGoBack(PRBool *aCanGoBack)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean canGoForward; */
NS_IMETHODIMP nsWebNavigation::GetCanGoForward(PRBool *aCanGoForward)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void goBack (); */
NS_IMETHODIMP nsWebNavigation::GoBack()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void goForward (); */
NS_IMETHODIMP nsWebNavigation::GoForward()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void gotoIndex (in long index); */
NS_IMETHODIMP nsWebNavigation::GotoIndex(PRInt32 index)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void loadURI (in wstring uri, in unsigned long load_flags, in nsIURI referrer, in nsIInputStream post_data, in nsIInputStream headers); */
NS_IMETHODIMP nsWebNavigation::LoadURI(const PRUnichar *uri, PRUint32 load_flags, nsIURI *referrer, nsIInputStream *post_data, nsIInputStream *headers)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void reload (in unsigned long reload_flags); */
NS_IMETHODIMP nsWebNavigation::Reload(PRUint32 reload_flags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void stop (in unsigned long stop_flags); */
NS_IMETHODIMP nsWebNavigation::Stop(PRUint32 stop_flags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIDOMDocument document; */
NS_IMETHODIMP nsWebNavigation::GetDocument(nsIDOMDocument * *aDocument)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIURI currentURI; */
NS_IMETHODIMP nsWebNavigation::GetCurrentURI(nsIURI * *aCurrentURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIURI referringURI; */
NS_IMETHODIMP nsWebNavigation::GetReferringURI(nsIURI * *aReferringURI)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsISHistory sessionHistory; */
NS_IMETHODIMP nsWebNavigation::GetSessionHistory(nsISHistory * *aSessionHistory)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWebNavigation::SetSessionHistory(nsISHistory * aSessionHistory)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIWebBrowserChrome; /* forward declaration */


/* starting interface:    nsIWindowCreator */
#define NS_IWINDOWCREATOR_IID_STR "30465632-a777-44cc-90f9-8145475ef999"

#define NS_IWINDOWCREATOR_IID \
  {0x30465632, 0xa777, 0x44cc, \
    { 0x90, 0xf9, 0x81, 0x45, 0x47, 0x5e, 0xf9, 0x99 }}

class NS_NO_VTABLE nsIWindowCreator : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWINDOWCREATOR_IID)

  /* nsIWebBrowserChrome createChromeWindow (in nsIWebBrowserChrome parent, in unsigned long chrome_flags); */
  NS_IMETHOD CreateChromeWindow(nsIWebBrowserChrome *parent, PRUint32 chrome_flags, nsIWebBrowserChrome **_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWINDOWCREATOR \
  NS_IMETHOD CreateChromeWindow(nsIWebBrowserChrome *parent, PRUint32 chrome_flags, nsIWebBrowserChrome **_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWINDOWCREATOR(_to) \
  NS_IMETHOD CreateChromeWindow(nsIWebBrowserChrome *parent, PRUint32 chrome_flags, nsIWebBrowserChrome **_retval) { return _to CreateChromeWindow(parent, chrome_flags, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWINDOWCREATOR(_to) \
  NS_IMETHOD CreateChromeWindow(nsIWebBrowserChrome *parent, PRUint32 chrome_flags, nsIWebBrowserChrome **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateChromeWindow(parent, chrome_flags, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWindowCreator : public nsIWindowCreator
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWINDOWCREATOR

  nsWindowCreator();

private:
  ~nsWindowCreator();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWindowCreator, nsIWindowCreator)

nsWindowCreator::nsWindowCreator()
{
  /* member initializers and constructor code */
}

nsWindowCreator::~nsWindowCreator()
{
  /* destructor code */
}

/* nsIWebBrowserChrome createChromeWindow (in nsIWebBrowserChrome parent, in unsigned long chrome_flags); */
NS_IMETHODIMP nsWindowCreator::CreateChromeWindow(nsIWebBrowserChrome *parent, PRUint32 chrome_flags, nsIWebBrowserChrome **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIDOMWindow; /* forward declaration */

class nsIObserver; /* forward declaration */

class nsISimpleEnumerator; /* forward declaration */

class nsIAuthPrompt; /* forward declaration */

class nsIWebBrowserChrome; /* forward declaration */

class nsIPrompt; /* forward declaration */

class nsIWindowCreator; /* forward declaration */


/* starting interface:    nsIWindowWatcher */
#define NS_IWINDOWWATCHER_IID_STR "002286a8-494b-43b3-8ddd-49e3fc50622b"

#define NS_IWINDOWWATCHER_IID \
  {0x002286a8, 0x494b, 0x43b3, \
    { 0x8d, 0xdd, 0x49, 0xe3, 0xfc, 0x50, 0x62, 0x2b }}

class NS_NO_VTABLE nsIWindowWatcher : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWINDOWWATCHER_IID)

  /* nsIDOMWindow openWindow (in nsIDOMWindow parent, in string url, in string name, in string features, in nsISupports arguments); */
  NS_IMETHOD OpenWindow(nsIDOMWindow *parent, const char *url, const char *name, const char *features, nsISupports *arguments, nsIDOMWindow **_retval) = 0;

  /* void registerNotification (in nsIObserver observer); */
  NS_IMETHOD RegisterNotification(nsIObserver *observer) = 0;

  /* void unregisterNotification (in nsIObserver observer); */
  NS_IMETHOD UnregisterNotification(nsIObserver *observer) = 0;

  /* nsISimpleEnumerator getWindowEnumerator (); */
  NS_IMETHOD GetWindowEnumerator(nsISimpleEnumerator **_retval) = 0;

  /* nsIPrompt getNewPrompter (in nsIDOMWindow parent); */
  NS_IMETHOD GetNewPrompter(nsIDOMWindow *parent, nsIPrompt **_retval) = 0;

  /* nsIAuthPrompt getNewAuthPrompter (in nsIDOMWindow parent); */
  NS_IMETHOD GetNewAuthPrompter(nsIDOMWindow *parent, nsIAuthPrompt **_retval) = 0;

  /* void setWindowCreator (in nsIWindowCreator creator); */
  NS_IMETHOD SetWindowCreator(nsIWindowCreator *creator) = 0;

  /* nsIWebBrowserChrome getChromeForWindow (in nsIDOMWindow window); */
  NS_IMETHOD GetChromeForWindow(nsIDOMWindow *window, nsIWebBrowserChrome **_retval) = 0;

  /* nsIDOMWindow getWindowByName (in wstring target_name, in nsIDOMWindow current_window); */
  NS_IMETHOD GetWindowByName(const PRUnichar *target_name, nsIDOMWindow *current_window, nsIDOMWindow **_retval) = 0;

  /* attribute nsIDOMWindow activeWindow; */
  NS_IMETHOD GetActiveWindow(nsIDOMWindow * *aActiveWindow) = 0;
  NS_IMETHOD SetActiveWindow(nsIDOMWindow * aActiveWindow) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIWINDOWWATCHER \
  NS_IMETHOD OpenWindow(nsIDOMWindow *parent, const char *url, const char *name, const char *features, nsISupports *arguments, nsIDOMWindow **_retval); \
  NS_IMETHOD RegisterNotification(nsIObserver *observer); \
  NS_IMETHOD UnregisterNotification(nsIObserver *observer); \
  NS_IMETHOD GetWindowEnumerator(nsISimpleEnumerator **_retval); \
  NS_IMETHOD GetNewPrompter(nsIDOMWindow *parent, nsIPrompt **_retval); \
  NS_IMETHOD GetNewAuthPrompter(nsIDOMWindow *parent, nsIAuthPrompt **_retval); \
  NS_IMETHOD SetWindowCreator(nsIWindowCreator *creator); \
  NS_IMETHOD GetChromeForWindow(nsIDOMWindow *window, nsIWebBrowserChrome **_retval); \
  NS_IMETHOD GetWindowByName(const PRUnichar *target_name, nsIDOMWindow *current_window, nsIDOMWindow **_retval); \
  NS_IMETHOD GetActiveWindow(nsIDOMWindow * *aActiveWindow); \
  NS_IMETHOD SetActiveWindow(nsIDOMWindow * aActiveWindow); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIWINDOWWATCHER(_to) \
  NS_IMETHOD OpenWindow(nsIDOMWindow *parent, const char *url, const char *name, const char *features, nsISupports *arguments, nsIDOMWindow **_retval) { return _to OpenWindow(parent, url, name, features, arguments, _retval); } \
  NS_IMETHOD RegisterNotification(nsIObserver *observer) { return _to RegisterNotification(observer); } \
  NS_IMETHOD UnregisterNotification(nsIObserver *observer) { return _to UnregisterNotification(observer); } \
  NS_IMETHOD GetWindowEnumerator(nsISimpleEnumerator **_retval) { return _to GetWindowEnumerator(_retval); } \
  NS_IMETHOD GetNewPrompter(nsIDOMWindow *parent, nsIPrompt **_retval) { return _to GetNewPrompter(parent, _retval); } \
  NS_IMETHOD GetNewAuthPrompter(nsIDOMWindow *parent, nsIAuthPrompt **_retval) { return _to GetNewAuthPrompter(parent, _retval); } \
  NS_IMETHOD SetWindowCreator(nsIWindowCreator *creator) { return _to SetWindowCreator(creator); } \
  NS_IMETHOD GetChromeForWindow(nsIDOMWindow *window, nsIWebBrowserChrome **_retval) { return _to GetChromeForWindow(window, _retval); } \
  NS_IMETHOD GetWindowByName(const PRUnichar *target_name, nsIDOMWindow *current_window, nsIDOMWindow **_retval) { return _to GetWindowByName(target_name, current_window, _retval); } \
  NS_IMETHOD GetActiveWindow(nsIDOMWindow * *aActiveWindow) { return _to GetActiveWindow(aActiveWindow); } \
  NS_IMETHOD SetActiveWindow(nsIDOMWindow * aActiveWindow) { return _to SetActiveWindow(aActiveWindow); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIWINDOWWATCHER(_to) \
  NS_IMETHOD OpenWindow(nsIDOMWindow *parent, const char *url, const char *name, const char *features, nsISupports *arguments, nsIDOMWindow **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenWindow(parent, url, name, features, arguments, _retval); } \
  NS_IMETHOD RegisterNotification(nsIObserver *observer) { return !_to ? NS_ERROR_NULL_POINTER : _to->RegisterNotification(observer); } \
  NS_IMETHOD UnregisterNotification(nsIObserver *observer) { return !_to ? NS_ERROR_NULL_POINTER : _to->UnregisterNotification(observer); } \
  NS_IMETHOD GetWindowEnumerator(nsISimpleEnumerator **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWindowEnumerator(_retval); } \
  NS_IMETHOD GetNewPrompter(nsIDOMWindow *parent, nsIPrompt **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNewPrompter(parent, _retval); } \
  NS_IMETHOD GetNewAuthPrompter(nsIDOMWindow *parent, nsIAuthPrompt **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNewAuthPrompter(parent, _retval); } \
  NS_IMETHOD SetWindowCreator(nsIWindowCreator *creator) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetWindowCreator(creator); } \
  NS_IMETHOD GetChromeForWindow(nsIDOMWindow *window, nsIWebBrowserChrome **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetChromeForWindow(window, _retval); } \
  NS_IMETHOD GetWindowByName(const PRUnichar *target_name, nsIDOMWindow *current_window, nsIDOMWindow **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetWindowByName(target_name, current_window, _retval); } \
  NS_IMETHOD GetActiveWindow(nsIDOMWindow * *aActiveWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetActiveWindow(aActiveWindow); } \
  NS_IMETHOD SetActiveWindow(nsIDOMWindow * aActiveWindow) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetActiveWindow(aActiveWindow); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsWindowWatcher : public nsIWindowWatcher
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIWINDOWWATCHER

  nsWindowWatcher();

private:
  ~nsWindowWatcher();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsWindowWatcher, nsIWindowWatcher)

nsWindowWatcher::nsWindowWatcher()
{
  /* member initializers and constructor code */
}

nsWindowWatcher::~nsWindowWatcher()
{
  /* destructor code */
}

/* nsIDOMWindow openWindow (in nsIDOMWindow parent, in string url, in string name, in string features, in nsISupports arguments); */
NS_IMETHODIMP nsWindowWatcher::OpenWindow(nsIDOMWindow *parent, const char *url, const char *name, const char *features, nsISupports *arguments, nsIDOMWindow **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void registerNotification (in nsIObserver observer); */
NS_IMETHODIMP nsWindowWatcher::RegisterNotification(nsIObserver *observer)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void unregisterNotification (in nsIObserver observer); */
NS_IMETHODIMP nsWindowWatcher::UnregisterNotification(nsIObserver *observer)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsISimpleEnumerator getWindowEnumerator (); */
NS_IMETHODIMP nsWindowWatcher::GetWindowEnumerator(nsISimpleEnumerator **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrompt getNewPrompter (in nsIDOMWindow parent); */
NS_IMETHODIMP nsWindowWatcher::GetNewPrompter(nsIDOMWindow *parent, nsIPrompt **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIAuthPrompt getNewAuthPrompter (in nsIDOMWindow parent); */
NS_IMETHODIMP nsWindowWatcher::GetNewAuthPrompter(nsIDOMWindow *parent, nsIAuthPrompt **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setWindowCreator (in nsIWindowCreator creator); */
NS_IMETHODIMP nsWindowWatcher::SetWindowCreator(nsIWindowCreator *creator)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIWebBrowserChrome getChromeForWindow (in nsIDOMWindow window); */
NS_IMETHODIMP nsWindowWatcher::GetChromeForWindow(nsIDOMWindow *window, nsIWebBrowserChrome **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIDOMWindow getWindowByName (in wstring target_name, in nsIDOMWindow current_window); */
NS_IMETHODIMP nsWindowWatcher::GetWindowByName(const PRUnichar *target_name, nsIDOMWindow *current_window, nsIDOMWindow **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIDOMWindow activeWindow; */
NS_IMETHODIMP nsWindowWatcher::GetActiveWindow(nsIDOMWindow * *aActiveWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsWindowWatcher::SetActiveWindow(nsIDOMWindow * aActiveWindow)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

struct JSObject;
typedef long jsval;
class nsIClassInfo; /* forward declaration */

class nsIXPCNativeCallContext; /* forward declaration */


/* starting interface:    nsIXPCSecurityManager */
#define NS_IXPCSECURITYMANAGER_IID_STR "31431440-f1ce-11d2-985a-006008962422"

#define NS_IXPCSECURITYMANAGER_IID \
  {0x31431440, 0xf1ce, 0x11d2, \
    { 0x98, 0x5a, 0x00, 0x60, 0x08, 0x96, 0x24, 0x22 }}

class NS_NO_VTABLE nsIXPCSecurityManager : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IXPCSECURITYMANAGER_IID)

  /* void CanCreateWrapper (in JSContextPtr js_context, in nsIIDRef iid, in nsISupports obj, in nsIClassInfo class_info, inout voidPtr policy); */
  NS_IMETHOD CanCreateWrapper(JSContext * js_context, const nsIID & iid, nsISupports *obj, nsIClassInfo *class_info, void * *policy) = 0;

  /* void CanCreateInstance (in JSContextPtr js_context, in nsCIDRef cid); */
  NS_IMETHOD CanCreateInstance(JSContext * js_context, const nsCID & cid) = 0;

  /* void CanGetService (in JSContextPtr js_context, in nsCIDRef cid); */
  NS_IMETHOD CanGetService(JSContext * js_context, const nsCID & cid) = 0;

  /* void CanAccess (in PRUint32 action, in nsIXPCNativeCallContext call_context, in JSContextPtr js_context, in JSObjectPtr js_object, in nsISupports obj, in nsIClassInfo class_info, in JSVal name, inout voidPtr policy); */
  NS_IMETHOD CanAccess(PRUint32 action, nsIXPCNativeCallContext *call_context, JSContext * js_context, JSObject * js_object, nsISupports *obj, nsIClassInfo *class_info, jsval name, void * *policy) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIXPCSECURITYMANAGER \
  NS_IMETHOD CanCreateWrapper(JSContext * js_context, const nsIID & iid, nsISupports *obj, nsIClassInfo *class_info, void * *policy); \
  NS_IMETHOD CanCreateInstance(JSContext * js_context, const nsCID & cid); \
  NS_IMETHOD CanGetService(JSContext * js_context, const nsCID & cid); \
  NS_IMETHOD CanAccess(PRUint32 action, nsIXPCNativeCallContext *call_context, JSContext * js_context, JSObject * js_object, nsISupports *obj, nsIClassInfo *class_info, jsval name, void * *policy); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIXPCSECURITYMANAGER(_to) \
  NS_IMETHOD CanCreateWrapper(JSContext * js_context, const nsIID & iid, nsISupports *obj, nsIClassInfo *class_info, void * *policy) { return _to CanCreateWrapper(js_context, iid, obj, class_info, policy); } \
  NS_IMETHOD CanCreateInstance(JSContext * js_context, const nsCID & cid) { return _to CanCreateInstance(js_context, cid); } \
  NS_IMETHOD CanGetService(JSContext * js_context, const nsCID & cid) { return _to CanGetService(js_context, cid); } \
  NS_IMETHOD CanAccess(PRUint32 action, nsIXPCNativeCallContext *call_context, JSContext * js_context, JSObject * js_object, nsISupports *obj, nsIClassInfo *class_info, jsval name, void * *policy) { return _to CanAccess(action, call_context, js_context, js_object, obj, class_info, name, policy); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIXPCSECURITYMANAGER(_to) \
  NS_IMETHOD CanCreateWrapper(JSContext * js_context, const nsIID & iid, nsISupports *obj, nsIClassInfo *class_info, void * *policy) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanCreateWrapper(js_context, iid, obj, class_info, policy); } \
  NS_IMETHOD CanCreateInstance(JSContext * js_context, const nsCID & cid) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanCreateInstance(js_context, cid); } \
  NS_IMETHOD CanGetService(JSContext * js_context, const nsCID & cid) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanGetService(js_context, cid); } \
  NS_IMETHOD CanAccess(PRUint32 action, nsIXPCNativeCallContext *call_context, JSContext * js_context, JSObject * js_object, nsISupports *obj, nsIClassInfo *class_info, jsval name, void * *policy) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanAccess(action, call_context, js_context, js_object, obj, class_info, name, policy); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsXPCSecurityManager : public nsIXPCSecurityManager
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIXPCSECURITYMANAGER

  nsXPCSecurityManager();

private:
  ~nsXPCSecurityManager();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsXPCSecurityManager, nsIXPCSecurityManager)

nsXPCSecurityManager::nsXPCSecurityManager()
{
  /* member initializers and constructor code */
}

nsXPCSecurityManager::~nsXPCSecurityManager()
{
  /* destructor code */
}

/* void CanCreateWrapper (in JSContextPtr js_context, in nsIIDRef iid, in nsISupports obj, in nsIClassInfo class_info, inout voidPtr policy); */
NS_IMETHODIMP nsXPCSecurityManager::CanCreateWrapper(JSContext * js_context, const nsIID & iid, nsISupports *obj, nsIClassInfo *class_info, void * *policy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void CanCreateInstance (in JSContextPtr js_context, in nsCIDRef cid); */
NS_IMETHODIMP nsXPCSecurityManager::CanCreateInstance(JSContext * js_context, const nsCID & cid)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void CanGetService (in JSContextPtr js_context, in nsCIDRef cid); */
NS_IMETHODIMP nsXPCSecurityManager::CanGetService(JSContext * js_context, const nsCID & cid)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void CanAccess (in PRUint32 action, in nsIXPCNativeCallContext call_context, in JSContextPtr js_context, in JSObjectPtr js_object, in nsISupports obj, in nsIClassInfo class_info, in JSVal name, inout voidPtr policy); */
NS_IMETHODIMP nsXPCSecurityManager::CanAccess(PRUint32 action, nsIXPCNativeCallContext *call_context, JSContext * js_context, JSObject * js_object, nsISupports *obj, nsIClassInfo *class_info, jsval name, void * *policy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

class nsIURI; /* forward declaration */


/* starting interface:    nsIPrincipal */
#define NS_IPRINCIPAL_IID_STR "0575ea96-4561-4dc6-a818-3c4c97c2430d"

#define NS_IPRINCIPAL_IID \
  {0x0575ea96, 0x4561, 0x4dc6, \
    { 0xa8, 0x18, 0x3c, 0x4c, 0x97, 0xc2, 0x43, 0x0d }}

class NS_NO_VTABLE nsIPrincipal : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IPRINCIPAL_IID)

  /* void placeholder (); */
  NS_IMETHOD Placeholder(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPRINCIPAL \
  NS_IMETHOD Placeholder(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPRINCIPAL(_to) \
  NS_IMETHOD Placeholder(void) { return _to Placeholder(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPRINCIPAL(_to) \
  NS_IMETHOD Placeholder(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Placeholder(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsPrincipal : public nsIPrincipal
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPRINCIPAL

  nsPrincipal();

private:
  ~nsPrincipal();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsPrincipal, nsIPrincipal)

nsPrincipal::nsPrincipal()
{
  /* member initializers and constructor code */
}

nsPrincipal::~nsPrincipal()
{
  /* destructor code */
}

/* void placeholder (); */
NS_IMETHODIMP nsPrincipal::Placeholder()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIScriptSecurityManager */
#define NS_ISCRIPTSECURITYMANAGER_IID_STR "f4d74511-2b2d-4a14-a3e4-a392ac5ac3ff"

#define NS_ISCRIPTSECURITYMANAGER_IID \
  {0xf4d74511, 0x2b2d, 0x4a14, \
    { 0xa3, 0xe4, 0xa3, 0x92, 0xac, 0x5a, 0xc3, 0xff }}

class NS_NO_VTABLE nsIScriptSecurityManager : public nsIXPCSecurityManager {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISCRIPTSECURITYMANAGER_IID)

  /* void checkPropertyAccess (in JSContextPtr js_context, in JSObjectPtr js_object, in string class_name, in JSVal property, in PRUint32 action); */
  NS_IMETHOD CheckPropertyAccess(JSContext * js_context, JSObject * js_object, const char *class_name, jsval property, PRUint32 action) = 0;

  /* void checkConnect (in JSContextPtr js_context, in nsIURI target_uri, in string class_name, in string property); */
  NS_IMETHOD CheckConnect(JSContext * js_context, nsIURI *target_uri, const char *class_name, const char *property) = 0;

  /* void checkLoadURIFromScript (in JSContextPtr cx, in nsIURI uri); */
  NS_IMETHOD CheckLoadURIFromScript(JSContext * cx, nsIURI *uri) = 0;

  /* void checkLoadURIWithPrincipal (in nsIPrincipal principal, in nsIURI uri, in unsigned long flags); */
  NS_IMETHOD CheckLoadURIWithPrincipal(nsIPrincipal *principal, nsIURI *uri, PRUint32 flags) = 0;

  /* void checkLoadURI (in nsIURI from, in nsIURI uri, in unsigned long flags); */
  NS_IMETHOD CheckLoadURI(nsIURI *from, nsIURI *uri, PRUint32 flags) = 0;

  /* void checkLoadURIStr (in AUTF8String from, in AUTF8String uri, in unsigned long flags); */
  NS_IMETHOD CheckLoadURIStr(const nsACString & from, const nsACString & uri, PRUint32 flags) = 0;

  /* void checkFunctionAccess (in JSContextPtr cx, in voidPtr fun_obj, in voidPtr target_obj); */
  NS_IMETHOD CheckFunctionAccess(JSContext * cx, void * fun_obj, void * target_obj) = 0;

  /* boolean canExecuteScripts (in JSContextPtr cx, in nsIPrincipal principal); */
  NS_IMETHOD CanExecuteScripts(JSContext * cx, nsIPrincipal *principal, PRBool *_retval) = 0;

  /* nsIPrincipal getSubjectPrincipal (); */
  NS_IMETHOD GetSubjectPrincipal(nsIPrincipal **_retval) = 0;

  /* nsIPrincipal getSystemPrincipal (); */
  NS_IMETHOD GetSystemPrincipal(nsIPrincipal **_retval) = 0;

  /* nsIPrincipal getCertificatePrincipal (in AUTF8String cert_fingerprint, in AUTF8String subject_name, in AUTF8String pretty_name, in nsISupports cert, in nsIURI uri); */
  NS_IMETHOD GetCertificatePrincipal(const nsACString & cert_fingerprint, const nsACString & subject_name, const nsACString & pretty_name, nsISupports *cert, nsIURI *uri, nsIPrincipal **_retval) = 0;

  /* nsIPrincipal getCodebasePrincipal (in nsIURI uri); */
  NS_IMETHOD GetCodebasePrincipal(nsIURI *uri, nsIPrincipal **_retval) = 0;

  /* short requestCapability (in nsIPrincipal principal, in string capability); */
  NS_IMETHOD RequestCapability(nsIPrincipal *principal, const char *capability, PRInt16 *_retval) = 0;

  /* boolean isCapabilityEnabled (in string capability); */
  NS_IMETHOD IsCapabilityEnabled(const char *capability, PRBool *_retval) = 0;

  /* void enableCapability (in string capability); */
  NS_IMETHOD EnableCapability(const char *capability) = 0;

  /* void revertCapability (in string capability); */
  NS_IMETHOD RevertCapability(const char *capability) = 0;

  /* void disableCapability (in string capability); */
  NS_IMETHOD DisableCapability(const char *capability) = 0;

  /* void setCanEnableCapability (in AUTF8String certificate_id, in string capability, in short can_enable); */
  NS_IMETHOD SetCanEnableCapability(const nsACString & certificate_id, const char *capability, PRInt16 can_enable) = 0;

  /* nsIPrincipal getObjectPrincipal (in JSContextPtr cx, in JSObjectPtr obj); */
  NS_IMETHOD GetObjectPrincipal(JSContext * cx, JSObject * obj, nsIPrincipal **_retval) = 0;

  /* boolean subjectPrincipalIsSystem (); */
  NS_IMETHOD SubjectPrincipalIsSystem(PRBool *_retval) = 0;

  /* void checkSameOrigin (in JSContextPtr js_context, in nsIURI target_uri); */
  NS_IMETHOD CheckSameOrigin(JSContext * js_context, nsIURI *target_uri) = 0;

  /* void checkSameOriginURI (in nsIURI source_uri, in nsIURI target_uri); */
  NS_IMETHOD CheckSameOriginURI(nsIURI *source_uri, nsIURI *target_uri) = 0;

  /* void checkSameOriginPrincipal (in nsIPrincipal source_principal, in nsIPrincipal target_principal); */
  NS_IMETHOD CheckSameOriginPrincipal(nsIPrincipal *source_principal, nsIPrincipal *target_principal) = 0;

  /* nsIPrincipal getPrincipalFromContext (in JSContextPtr cx); */
  NS_IMETHOD GetPrincipalFromContext(JSContext * cx, nsIPrincipal **_retval) = 0;

  /* boolean securityCompareURIs (in nsIURI subject_uri, in nsIURI object_uri); */
  NS_IMETHOD SecurityCompareURIs(nsIURI *subject_uri, nsIURI *object_uri, PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISCRIPTSECURITYMANAGER \
  NS_IMETHOD CheckPropertyAccess(JSContext * js_context, JSObject * js_object, const char *class_name, jsval property, PRUint32 action); \
  NS_IMETHOD CheckConnect(JSContext * js_context, nsIURI *target_uri, const char *class_name, const char *property); \
  NS_IMETHOD CheckLoadURIFromScript(JSContext * cx, nsIURI *uri); \
  NS_IMETHOD CheckLoadURIWithPrincipal(nsIPrincipal *principal, nsIURI *uri, PRUint32 flags); \
  NS_IMETHOD CheckLoadURI(nsIURI *from, nsIURI *uri, PRUint32 flags); \
  NS_IMETHOD CheckLoadURIStr(const nsACString & from, const nsACString & uri, PRUint32 flags); \
  NS_IMETHOD CheckFunctionAccess(JSContext * cx, void * fun_obj, void * target_obj); \
  NS_IMETHOD CanExecuteScripts(JSContext * cx, nsIPrincipal *principal, PRBool *_retval); \
  NS_IMETHOD GetSubjectPrincipal(nsIPrincipal **_retval); \
  NS_IMETHOD GetSystemPrincipal(nsIPrincipal **_retval); \
  NS_IMETHOD GetCertificatePrincipal(const nsACString & cert_fingerprint, const nsACString & subject_name, const nsACString & pretty_name, nsISupports *cert, nsIURI *uri, nsIPrincipal **_retval); \
  NS_IMETHOD GetCodebasePrincipal(nsIURI *uri, nsIPrincipal **_retval); \
  NS_IMETHOD RequestCapability(nsIPrincipal *principal, const char *capability, PRInt16 *_retval); \
  NS_IMETHOD IsCapabilityEnabled(const char *capability, PRBool *_retval); \
  NS_IMETHOD EnableCapability(const char *capability); \
  NS_IMETHOD RevertCapability(const char *capability); \
  NS_IMETHOD DisableCapability(const char *capability); \
  NS_IMETHOD SetCanEnableCapability(const nsACString & certificate_id, const char *capability, PRInt16 can_enable); \
  NS_IMETHOD GetObjectPrincipal(JSContext * cx, JSObject * obj, nsIPrincipal **_retval); \
  NS_IMETHOD SubjectPrincipalIsSystem(PRBool *_retval); \
  NS_IMETHOD CheckSameOrigin(JSContext * js_context, nsIURI *target_uri); \
  NS_IMETHOD CheckSameOriginURI(nsIURI *source_uri, nsIURI *target_uri); \
  NS_IMETHOD CheckSameOriginPrincipal(nsIPrincipal *source_principal, nsIPrincipal *target_principal); \
  NS_IMETHOD GetPrincipalFromContext(JSContext * cx, nsIPrincipal **_retval); \
  NS_IMETHOD SecurityCompareURIs(nsIURI *subject_uri, nsIURI *object_uri, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISCRIPTSECURITYMANAGER(_to) \
  NS_IMETHOD CheckPropertyAccess(JSContext * js_context, JSObject * js_object, const char *class_name, jsval property, PRUint32 action) { return _to CheckPropertyAccess(js_context, js_object, class_name, property, action); } \
  NS_IMETHOD CheckConnect(JSContext * js_context, nsIURI *target_uri, const char *class_name, const char *property) { return _to CheckConnect(js_context, target_uri, class_name, property); } \
  NS_IMETHOD CheckLoadURIFromScript(JSContext * cx, nsIURI *uri) { return _to CheckLoadURIFromScript(cx, uri); } \
  NS_IMETHOD CheckLoadURIWithPrincipal(nsIPrincipal *principal, nsIURI *uri, PRUint32 flags) { return _to CheckLoadURIWithPrincipal(principal, uri, flags); } \
  NS_IMETHOD CheckLoadURI(nsIURI *from, nsIURI *uri, PRUint32 flags) { return _to CheckLoadURI(from, uri, flags); } \
  NS_IMETHOD CheckLoadURIStr(const nsACString & from, const nsACString & uri, PRUint32 flags) { return _to CheckLoadURIStr(from, uri, flags); } \
  NS_IMETHOD CheckFunctionAccess(JSContext * cx, void * fun_obj, void * target_obj) { return _to CheckFunctionAccess(cx, fun_obj, target_obj); } \
  NS_IMETHOD CanExecuteScripts(JSContext * cx, nsIPrincipal *principal, PRBool *_retval) { return _to CanExecuteScripts(cx, principal, _retval); } \
  NS_IMETHOD GetSubjectPrincipal(nsIPrincipal **_retval) { return _to GetSubjectPrincipal(_retval); } \
  NS_IMETHOD GetSystemPrincipal(nsIPrincipal **_retval) { return _to GetSystemPrincipal(_retval); } \
  NS_IMETHOD GetCertificatePrincipal(const nsACString & cert_fingerprint, const nsACString & subject_name, const nsACString & pretty_name, nsISupports *cert, nsIURI *uri, nsIPrincipal **_retval) { return _to GetCertificatePrincipal(cert_fingerprint, subject_name, pretty_name, cert, uri, _retval); } \
  NS_IMETHOD GetCodebasePrincipal(nsIURI *uri, nsIPrincipal **_retval) { return _to GetCodebasePrincipal(uri, _retval); } \
  NS_IMETHOD RequestCapability(nsIPrincipal *principal, const char *capability, PRInt16 *_retval) { return _to RequestCapability(principal, capability, _retval); } \
  NS_IMETHOD IsCapabilityEnabled(const char *capability, PRBool *_retval) { return _to IsCapabilityEnabled(capability, _retval); } \
  NS_IMETHOD EnableCapability(const char *capability) { return _to EnableCapability(capability); } \
  NS_IMETHOD RevertCapability(const char *capability) { return _to RevertCapability(capability); } \
  NS_IMETHOD DisableCapability(const char *capability) { return _to DisableCapability(capability); } \
  NS_IMETHOD SetCanEnableCapability(const nsACString & certificate_id, const char *capability, PRInt16 can_enable) { return _to SetCanEnableCapability(certificate_id, capability, can_enable); } \
  NS_IMETHOD GetObjectPrincipal(JSContext * cx, JSObject * obj, nsIPrincipal **_retval) { return _to GetObjectPrincipal(cx, obj, _retval); } \
  NS_IMETHOD SubjectPrincipalIsSystem(PRBool *_retval) { return _to SubjectPrincipalIsSystem(_retval); } \
  NS_IMETHOD CheckSameOrigin(JSContext * js_context, nsIURI *target_uri) { return _to CheckSameOrigin(js_context, target_uri); } \
  NS_IMETHOD CheckSameOriginURI(nsIURI *source_uri, nsIURI *target_uri) { return _to CheckSameOriginURI(source_uri, target_uri); } \
  NS_IMETHOD CheckSameOriginPrincipal(nsIPrincipal *source_principal, nsIPrincipal *target_principal) { return _to CheckSameOriginPrincipal(source_principal, target_principal); } \
  NS_IMETHOD GetPrincipalFromContext(JSContext * cx, nsIPrincipal **_retval) { return _to GetPrincipalFromContext(cx, _retval); } \
  NS_IMETHOD SecurityCompareURIs(nsIURI *subject_uri, nsIURI *object_uri, PRBool *_retval) { return _to SecurityCompareURIs(subject_uri, object_uri, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISCRIPTSECURITYMANAGER(_to) \
  NS_IMETHOD CheckPropertyAccess(JSContext * js_context, JSObject * js_object, const char *class_name, jsval property, PRUint32 action) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckPropertyAccess(js_context, js_object, class_name, property, action); } \
  NS_IMETHOD CheckConnect(JSContext * js_context, nsIURI *target_uri, const char *class_name, const char *property) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckConnect(js_context, target_uri, class_name, property); } \
  NS_IMETHOD CheckLoadURIFromScript(JSContext * cx, nsIURI *uri) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckLoadURIFromScript(cx, uri); } \
  NS_IMETHOD CheckLoadURIWithPrincipal(nsIPrincipal *principal, nsIURI *uri, PRUint32 flags) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckLoadURIWithPrincipal(principal, uri, flags); } \
  NS_IMETHOD CheckLoadURI(nsIURI *from, nsIURI *uri, PRUint32 flags) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckLoadURI(from, uri, flags); } \
  NS_IMETHOD CheckLoadURIStr(const nsACString & from, const nsACString & uri, PRUint32 flags) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckLoadURIStr(from, uri, flags); } \
  NS_IMETHOD CheckFunctionAccess(JSContext * cx, void * fun_obj, void * target_obj) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckFunctionAccess(cx, fun_obj, target_obj); } \
  NS_IMETHOD CanExecuteScripts(JSContext * cx, nsIPrincipal *principal, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->CanExecuteScripts(cx, principal, _retval); } \
  NS_IMETHOD GetSubjectPrincipal(nsIPrincipal **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSubjectPrincipal(_retval); } \
  NS_IMETHOD GetSystemPrincipal(nsIPrincipal **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSystemPrincipal(_retval); } \
  NS_IMETHOD GetCertificatePrincipal(const nsACString & cert_fingerprint, const nsACString & subject_name, const nsACString & pretty_name, nsISupports *cert, nsIURI *uri, nsIPrincipal **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCertificatePrincipal(cert_fingerprint, subject_name, pretty_name, cert, uri, _retval); } \
  NS_IMETHOD GetCodebasePrincipal(nsIURI *uri, nsIPrincipal **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCodebasePrincipal(uri, _retval); } \
  NS_IMETHOD RequestCapability(nsIPrincipal *principal, const char *capability, PRInt16 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->RequestCapability(principal, capability, _retval); } \
  NS_IMETHOD IsCapabilityEnabled(const char *capability, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsCapabilityEnabled(capability, _retval); } \
  NS_IMETHOD EnableCapability(const char *capability) { return !_to ? NS_ERROR_NULL_POINTER : _to->EnableCapability(capability); } \
  NS_IMETHOD RevertCapability(const char *capability) { return !_to ? NS_ERROR_NULL_POINTER : _to->RevertCapability(capability); } \
  NS_IMETHOD DisableCapability(const char *capability) { return !_to ? NS_ERROR_NULL_POINTER : _to->DisableCapability(capability); } \
  NS_IMETHOD SetCanEnableCapability(const nsACString & certificate_id, const char *capability, PRInt16 can_enable) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetCanEnableCapability(certificate_id, capability, can_enable); } \
  NS_IMETHOD GetObjectPrincipal(JSContext * cx, JSObject * obj, nsIPrincipal **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetObjectPrincipal(cx, obj, _retval); } \
  NS_IMETHOD SubjectPrincipalIsSystem(PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SubjectPrincipalIsSystem(_retval); } \
  NS_IMETHOD CheckSameOrigin(JSContext * js_context, nsIURI *target_uri) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckSameOrigin(js_context, target_uri); } \
  NS_IMETHOD CheckSameOriginURI(nsIURI *source_uri, nsIURI *target_uri) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckSameOriginURI(source_uri, target_uri); } \
  NS_IMETHOD CheckSameOriginPrincipal(nsIPrincipal *source_principal, nsIPrincipal *target_principal) { return !_to ? NS_ERROR_NULL_POINTER : _to->CheckSameOriginPrincipal(source_principal, target_principal); } \
  NS_IMETHOD GetPrincipalFromContext(JSContext * cx, nsIPrincipal **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrincipalFromContext(cx, _retval); } \
  NS_IMETHOD SecurityCompareURIs(nsIURI *subject_uri, nsIURI *object_uri, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->SecurityCompareURIs(subject_uri, object_uri, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsScriptSecurityManager : public nsIScriptSecurityManager
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISCRIPTSECURITYMANAGER

  nsScriptSecurityManager();

private:
  ~nsScriptSecurityManager();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsScriptSecurityManager, nsIScriptSecurityManager)

nsScriptSecurityManager::nsScriptSecurityManager()
{
  /* member initializers and constructor code */
}

nsScriptSecurityManager::~nsScriptSecurityManager()
{
  /* destructor code */
}

/* void checkPropertyAccess (in JSContextPtr js_context, in JSObjectPtr js_object, in string class_name, in JSVal property, in PRUint32 action); */
NS_IMETHODIMP nsScriptSecurityManager::CheckPropertyAccess(JSContext * js_context, JSObject * js_object, const char *class_name, jsval property, PRUint32 action)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkConnect (in JSContextPtr js_context, in nsIURI target_uri, in string class_name, in string property); */
NS_IMETHODIMP nsScriptSecurityManager::CheckConnect(JSContext * js_context, nsIURI *target_uri, const char *class_name, const char *property)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkLoadURIFromScript (in JSContextPtr cx, in nsIURI uri); */
NS_IMETHODIMP nsScriptSecurityManager::CheckLoadURIFromScript(JSContext * cx, nsIURI *uri)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkLoadURIWithPrincipal (in nsIPrincipal principal, in nsIURI uri, in unsigned long flags); */
NS_IMETHODIMP nsScriptSecurityManager::CheckLoadURIWithPrincipal(nsIPrincipal *principal, nsIURI *uri, PRUint32 flags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkLoadURI (in nsIURI from, in nsIURI uri, in unsigned long flags); */
NS_IMETHODIMP nsScriptSecurityManager::CheckLoadURI(nsIURI *from, nsIURI *uri, PRUint32 flags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkLoadURIStr (in AUTF8String from, in AUTF8String uri, in unsigned long flags); */
NS_IMETHODIMP nsScriptSecurityManager::CheckLoadURIStr(const nsACString & from, const nsACString & uri, PRUint32 flags)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkFunctionAccess (in JSContextPtr cx, in voidPtr fun_obj, in voidPtr target_obj); */
NS_IMETHODIMP nsScriptSecurityManager::CheckFunctionAccess(JSContext * cx, void * fun_obj, void * target_obj)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean canExecuteScripts (in JSContextPtr cx, in nsIPrincipal principal); */
NS_IMETHODIMP nsScriptSecurityManager::CanExecuteScripts(JSContext * cx, nsIPrincipal *principal, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrincipal getSubjectPrincipal (); */
NS_IMETHODIMP nsScriptSecurityManager::GetSubjectPrincipal(nsIPrincipal **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrincipal getSystemPrincipal (); */
NS_IMETHODIMP nsScriptSecurityManager::GetSystemPrincipal(nsIPrincipal **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrincipal getCertificatePrincipal (in AUTF8String cert_fingerprint, in AUTF8String subject_name, in AUTF8String pretty_name, in nsISupports cert, in nsIURI uri); */
NS_IMETHODIMP nsScriptSecurityManager::GetCertificatePrincipal(const nsACString & cert_fingerprint, const nsACString & subject_name, const nsACString & pretty_name, nsISupports *cert, nsIURI *uri, nsIPrincipal **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrincipal getCodebasePrincipal (in nsIURI uri); */
NS_IMETHODIMP nsScriptSecurityManager::GetCodebasePrincipal(nsIURI *uri, nsIPrincipal **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* short requestCapability (in nsIPrincipal principal, in string capability); */
NS_IMETHODIMP nsScriptSecurityManager::RequestCapability(nsIPrincipal *principal, const char *capability, PRInt16 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isCapabilityEnabled (in string capability); */
NS_IMETHODIMP nsScriptSecurityManager::IsCapabilityEnabled(const char *capability, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void enableCapability (in string capability); */
NS_IMETHODIMP nsScriptSecurityManager::EnableCapability(const char *capability)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void revertCapability (in string capability); */
NS_IMETHODIMP nsScriptSecurityManager::RevertCapability(const char *capability)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void disableCapability (in string capability); */
NS_IMETHODIMP nsScriptSecurityManager::DisableCapability(const char *capability)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setCanEnableCapability (in AUTF8String certificate_id, in string capability, in short can_enable); */
NS_IMETHODIMP nsScriptSecurityManager::SetCanEnableCapability(const nsACString & certificate_id, const char *capability, PRInt16 can_enable)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrincipal getObjectPrincipal (in JSContextPtr cx, in JSObjectPtr obj); */
NS_IMETHODIMP nsScriptSecurityManager::GetObjectPrincipal(JSContext * cx, JSObject * obj, nsIPrincipal **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean subjectPrincipalIsSystem (); */
NS_IMETHODIMP nsScriptSecurityManager::SubjectPrincipalIsSystem(PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkSameOrigin (in JSContextPtr js_context, in nsIURI target_uri); */
NS_IMETHODIMP nsScriptSecurityManager::CheckSameOrigin(JSContext * js_context, nsIURI *target_uri)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkSameOriginURI (in nsIURI source_uri, in nsIURI target_uri); */
NS_IMETHODIMP nsScriptSecurityManager::CheckSameOriginURI(nsIURI *source_uri, nsIURI *target_uri)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void checkSameOriginPrincipal (in nsIPrincipal source_principal, in nsIPrincipal target_principal); */
NS_IMETHODIMP nsScriptSecurityManager::CheckSameOriginPrincipal(nsIPrincipal *source_principal, nsIPrincipal *target_principal)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIPrincipal getPrincipalFromContext (in JSContextPtr cx); */
NS_IMETHODIMP nsScriptSecurityManager::GetPrincipalFromContext(JSContext * cx, nsIPrincipal **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean securityCompareURIs (in nsIURI subject_uri, in nsIURI object_uri); */
NS_IMETHODIMP nsScriptSecurityManager::SecurityCompareURIs(nsIURI *subject_uri, nsIURI *object_uri, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsall_h__ */
