/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * The derived types of the type-system structure
 * Tsu (struct/union) depends on Stab which depends on Type, so we
 * split the type file into two.
 *
 * $Id: type2.h,v 1.14 2003/08/06 17:12:03 dds Exp $
 */

#ifndef TYPE2_
#define TYPE2_

// Array of ...
class Tarray: public Type_node {
private:
	Type of;
public:
	Tarray(Type t) : of(t) {}
	virtual ~Tarray() {}
	Type clone() const { return Type(new Tarray(of.clone())); }
	Type deref() const { return of; }
	Type subscript() const { return of; }
	bool is_ptr() const { return true; }
	bool is_array() const { return true; }
	void print(ostream &o) const;
	void set_abstract(Type t);
	void set_storage_class(Type t) { of.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return of.get_storage_class(); }
};

// Pointer to ...
class Tpointer: public Type_node {
private:
	Type to;
public:
	Tpointer(Type t) : to(t) {}
	virtual ~Tpointer() {}
	Type clone() const { return Type(new Tpointer(to.clone())); }
	Type deref() const { return to; }
	Type subscript() const { return to; }
	bool is_ptr() const { return true; }
	Type call() const { return to.call(); }
	void set_storage_class(Type t) { to.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return to.get_storage_class(); }
	void print(ostream &o) const;
	void set_abstract(Type t);
};

// Function returning ...
class Tfunction: public Type_node {
private:
	Type returning;
public:
	Tfunction(Type t) : returning(t) {}
	virtual ~Tfunction() {}
	Type clone() const { return Type(new Tfunction(returning.clone())); }
	Type call() const { return returning; }
	void print(ostream &o) const;
	void set_abstract(Type t);
	void set_storage_class(Type t) { returning.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return returning.get_storage_class(); }
};

// Enumeration
class Tenum: public Type_node {
private:
	Tstorage sclass;
public:
	Tenum(enum e_storage_class sc = c_unspecified) : sclass(sc) {}
	Type clone() const { return Type(new Tenum(sclass.get_storage_class()));}
	void print(ostream &o) const;
	enum e_storage_class get_storage_class() const { return sclass.get_storage_class(); }
	void set_storage_class(Type t) { sclass.set_storage_class(t); };
};

class Stab;

// Structure or Union
class Tsu: public Type_node {
private:
	Stab members_by_name;
	vector <Id> members_by_ordinal;
	Type default_specifier;	// Used while declaring a series of members
	Tstorage sclass;
public:
	Tsu(const Token &tok, const Type &typ, const Type &spec) { 
		tok.set_ec_attribute(is_sumember);
		if (DP()) cout << "Adding member " << tok << "\n";
		members_by_name.define(tok, typ); 
		members_by_ordinal.push_back(Id(tok, typ));
		default_specifier = spec; 
	}
	Tsu(const Stab &mbn, const vector <Id> &mbo, Type ds, enum e_storage_class sc) :
			members_by_name(mbn),
			members_by_ordinal(mbo),
			default_specifier(ds),
			sclass(sc) {}
	Tsu(const Type &spec) { default_specifier = spec; }
	Tsu() {}
	virtual ~Tsu() {}
	bool is_su() const { return true; }
	Type clone() const { return Type(new Tsu(members_by_name, members_by_ordinal, default_specifier.clone(), sclass.get_storage_class())); }
	void add_member(const Token &tok, const Type &typ) { 
		if (DP()) cout << "Adding member " << tok << "\n";
		tok.set_ec_attribute(is_sumember);
		members_by_name.define(tok, typ); 
		members_by_ordinal.push_back(Id(tok, typ));
	}
	Type get_default_specifier() const { return default_specifier; }
	void merge_with(Type t) { 
		members_by_name.merge_with(t.get_members_by_name());
		const vector <Id> &m2 = t.get_members_by_ordinal();
		members_by_ordinal.insert(members_by_ordinal.end(), m2.begin(), m2.end());
	}
	Id const* member(const string& s) const 
		{ return members_by_name.lookup(s); }
	Id const* member(unsigned n) const {
		if (n >= members_by_ordinal.size())
			return NULL;
		else
			return &(members_by_ordinal[n]);
	}
	const Stab& get_members_by_name() const { return members_by_name; }
	const vector <Id>& get_members_by_ordinal() const { return members_by_ordinal; }
	void print(ostream &o) const;
	enum e_storage_class get_storage_class() const { return sclass.get_storage_class(); }
	void set_storage_class(Type t) { sclass.set_storage_class(t); };
};
//
// Incomplete structure or union reference
class Tincomplete: public Type_node {
private:
	Ctoken t;
	Tstorage sclass;
	int scope_level;		// Level to lookup for complete definitions
public:
	Tincomplete(const Ctoken& tok, int l) : t(tok), scope_level(l) {}
	Tincomplete(const Ctoken& tok, enum e_storage_class sc, int l) : t(tok), sclass(sc), scope_level(l) {}
	virtual ~Tincomplete() {}
	Type clone() const { return Type(new Tincomplete(t, sclass.get_storage_class(), scope_level)); }
	Id const* member(const string& s) const;
	void print(ostream &o) const;
	const Ctoken& get_token() const { return t; }
	enum e_storage_class get_storage_class() const { return sclass.get_storage_class(); }
	void set_storage_class(Type t) { sclass.set_storage_class(t); };
	bool is_incomplete() const { return true; }
};


// Identifier; not really a type, it is returned by the lexical analyser
// It is also the type used to represent undeclared identifiers
class Tidentifier: public Type_node {
private:
	Ctoken t;
	Type of;			// Identifying a given type
public:
	Tidentifier(const Ctoken& tok) : t(tok), of(basic()) {}
	Tidentifier(const Ctoken& tok, Type typ) : t(tok), of(typ) {}
	virtual ~Tidentifier() {}
	Type clone() const { return Type(new Tidentifier(t, of.clone())); }
	const Ctoken& get_token() const { return t; }
	const string& get_name() const { return t.get_name(); }
	Type type() const { return of; }
	Id const* member(const string& s) const 
		{ return of.member(s); }
	Id const* member(unsigned n) const
		{ return of.member(n); }
	Type call() const;			// Function (undeclared)
	void print(ostream &o) const;
	void set_abstract(Type t);
	void set_storage_class(Type t) { of.set_storage_class(t); }
	enum e_storage_class get_storage_class() const {return of.get_storage_class(); }
};

// Goto label
class Tlabel: public Type_node {
public:
	Tlabel() {}
	void print(ostream &o) const;
};


#endif // TYPE2_
