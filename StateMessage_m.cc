//
// Generated file, do not edit! Created by opp_msgtool 6.0 from StateMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "StateMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Enum(MsgType, (MsgType::SET_SERVER_PHASE, MsgType::SET_N, MsgType::SET_Q));

Register_Enum(ServerPhase, (ServerPhase::VACATION, ServerPhase::CONNECTION, ServerPhase::SERVICE));

Register_Class(StateMessage)

StateMessage::StateMessage(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

StateMessage::StateMessage(const StateMessage& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

StateMessage::~StateMessage()
{
}

StateMessage& StateMessage::operator=(const StateMessage& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void StateMessage::copy(const StateMessage& other)
{
    this->msgType = other.msgType;
    this->serverPhase = other.serverPhase;
    this->queueIndex = other.queueIndex;
    this->q = other.q;
    this->n = other.n;
    this->kc = other.kc;
}

void StateMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->serverPhase);
    doParsimPacking(b,this->queueIndex);
    doParsimPacking(b,this->q);
    doParsimPacking(b,this->n);
    doParsimPacking(b,this->kc);
}

void StateMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->serverPhase);
    doParsimUnpacking(b,this->queueIndex);
    doParsimUnpacking(b,this->q);
    doParsimUnpacking(b,this->n);
    doParsimUnpacking(b,this->kc);
}

MsgType StateMessage::getMsgType() const
{
    return this->msgType;
}

void StateMessage::setMsgType(MsgType msgType)
{
    this->msgType = msgType;
}

ServerPhase StateMessage::getServerPhase() const
{
    return this->serverPhase;
}

void StateMessage::setServerPhase(ServerPhase serverPhase)
{
    this->serverPhase = serverPhase;
}

int StateMessage::getQueueIndex() const
{
    return this->queueIndex;
}

void StateMessage::setQueueIndex(int queueIndex)
{
    this->queueIndex = queueIndex;
}

int StateMessage::getQ() const
{
    return this->q;
}

void StateMessage::setQ(int q)
{
    this->q = q;
}

int StateMessage::getN() const
{
    return this->n;
}

void StateMessage::setN(int n)
{
    this->n = n;
}

int StateMessage::getKc() const
{
    return this->kc;
}

void StateMessage::setKc(int kc)
{
    this->kc = kc;
}

class StateMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_msgType,
        FIELD_serverPhase,
        FIELD_queueIndex,
        FIELD_q,
        FIELD_n,
        FIELD_kc,
    };
  public:
    StateMessageDescriptor();
    virtual ~StateMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(StateMessageDescriptor)

StateMessageDescriptor::StateMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(StateMessage)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

StateMessageDescriptor::~StateMessageDescriptor()
{
    delete[] propertyNames;
}

bool StateMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<StateMessage *>(obj)!=nullptr;
}

const char **StateMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *StateMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int StateMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 6+base->getFieldCount() : 6;
}

unsigned int StateMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        0,    // FIELD_msgType
        0,    // FIELD_serverPhase
        FD_ISEDITABLE,    // FIELD_queueIndex
        FD_ISEDITABLE,    // FIELD_q
        FD_ISEDITABLE,    // FIELD_n
        FD_ISEDITABLE,    // FIELD_kc
    };
    return (field >= 0 && field < 6) ? fieldTypeFlags[field] : 0;
}

const char *StateMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
        "serverPhase",
        "queueIndex",
        "q",
        "n",
        "kc",
    };
    return (field >= 0 && field < 6) ? fieldNames[field] : nullptr;
}

int StateMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "serverPhase") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "queueIndex") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "q") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "n") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "kc") == 0) return baseIndex + 5;
    return base ? base->findField(fieldName) : -1;
}

const char *StateMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "MsgType",    // FIELD_msgType
        "ServerPhase",    // FIELD_serverPhase
        "int",    // FIELD_queueIndex
        "int",    // FIELD_q
        "int",    // FIELD_n
        "int",    // FIELD_kc
    };
    return (field >= 0 && field < 6) ? fieldTypeStrings[field] : nullptr;
}

const char **StateMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_msgType: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        case FIELD_serverPhase: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *StateMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_msgType:
            if (!strcmp(propertyName, "enum")) return "MsgType";
            return nullptr;
        case FIELD_serverPhase:
            if (!strcmp(propertyName, "enum")) return "ServerPhase";
            return nullptr;
        default: return nullptr;
    }
}

int StateMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void StateMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'StateMessage'", field);
    }
}

const char *StateMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string StateMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return enum2string(pp->getMsgType(), "MsgType");
        case FIELD_serverPhase: return enum2string(pp->getServerPhase(), "ServerPhase");
        case FIELD_queueIndex: return long2string(pp->getQueueIndex());
        case FIELD_q: return long2string(pp->getQ());
        case FIELD_n: return long2string(pp->getN());
        case FIELD_kc: return long2string(pp->getKc());
        default: return "";
    }
}

void StateMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_queueIndex: pp->setQueueIndex(string2long(value)); break;
        case FIELD_q: pp->setQ(string2long(value)); break;
        case FIELD_n: pp->setN(string2long(value)); break;
        case FIELD_kc: pp->setKc(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'StateMessage'", field);
    }
}

omnetpp::cValue StateMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return static_cast<int>(pp->getMsgType());
        case FIELD_serverPhase: return static_cast<int>(pp->getServerPhase());
        case FIELD_queueIndex: return pp->getQueueIndex();
        case FIELD_q: return pp->getQ();
        case FIELD_n: return pp->getN();
        case FIELD_kc: return pp->getKc();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'StateMessage' as cValue -- field index out of range?", field);
    }
}

void StateMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        case FIELD_queueIndex: pp->setQueueIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_q: pp->setQ(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_n: pp->setN(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_kc: pp->setKc(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'StateMessage'", field);
    }
}

const char *StateMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr StateMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void StateMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    StateMessage *pp = omnetpp::fromAnyPtr<StateMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'StateMessage'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

