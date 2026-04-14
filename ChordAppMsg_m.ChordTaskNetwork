//
// Generated file, do not edit! Created by opp_msgtool 6.0 from ChordAppMsg.msg.
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
#include "ChordAppMsg_m.h"

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

namespace chord {

Register_Enum(chord::MsgType, (chord::MsgType::TASK_MSG, chord::MsgType::RESULT_MSG, chord::MsgType::GOSSIP_MSG));

Register_Class(ChordAppMsg)

ChordAppMsg::ChordAppMsg(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

ChordAppMsg::ChordAppMsg(const ChordAppMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

ChordAppMsg::~ChordAppMsg()
{
    delete [] this->data;
}

ChordAppMsg& ChordAppMsg::operator=(const ChordAppMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void ChordAppMsg::copy(const ChordAppMsg& other)
{
    this->msgType = other.msgType;
    this->sourceId = other.sourceId;
    this->destId = other.destId;
    this->initiatorId = other.initiatorId;
    this->subtaskId = other.subtaskId;
    delete [] this->data;
    this->data = (other.data_arraysize==0) ? nullptr : new int[other.data_arraysize];
    data_arraysize = other.data_arraysize;
    for (size_t i = 0; i < data_arraysize; i++) {
        this->data[i] = other.data[i];
    }
    this->maxResult = other.maxResult;
    this->gossipOriginIP = other.gossipOriginIP;
    this->gossipTimestamp = other.gossipTimestamp;
    this->gossipClientId = other.gossipClientId;
}

void ChordAppMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->msgType);
    doParsimPacking(b,this->sourceId);
    doParsimPacking(b,this->destId);
    doParsimPacking(b,this->initiatorId);
    doParsimPacking(b,this->subtaskId);
    b->pack(data_arraysize);
    doParsimArrayPacking(b,this->data,data_arraysize);
    doParsimPacking(b,this->maxResult);
    doParsimPacking(b,this->gossipOriginIP);
    doParsimPacking(b,this->gossipTimestamp);
    doParsimPacking(b,this->gossipClientId);
}

void ChordAppMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->msgType);
    doParsimUnpacking(b,this->sourceId);
    doParsimUnpacking(b,this->destId);
    doParsimUnpacking(b,this->initiatorId);
    doParsimUnpacking(b,this->subtaskId);
    delete [] this->data;
    b->unpack(data_arraysize);
    if (data_arraysize == 0) {
        this->data = nullptr;
    } else {
        this->data = new int[data_arraysize];
        doParsimArrayUnpacking(b,this->data,data_arraysize);
    }
    doParsimUnpacking(b,this->maxResult);
    doParsimUnpacking(b,this->gossipOriginIP);
    doParsimUnpacking(b,this->gossipTimestamp);
    doParsimUnpacking(b,this->gossipClientId);
}

int ChordAppMsg::getMsgType() const
{
    return this->msgType;
}

void ChordAppMsg::setMsgType(int msgType)
{
    this->msgType = msgType;
}

int ChordAppMsg::getSourceId() const
{
    return this->sourceId;
}

void ChordAppMsg::setSourceId(int sourceId)
{
    this->sourceId = sourceId;
}

int ChordAppMsg::getDestId() const
{
    return this->destId;
}

void ChordAppMsg::setDestId(int destId)
{
    this->destId = destId;
}

int ChordAppMsg::getInitiatorId() const
{
    return this->initiatorId;
}

void ChordAppMsg::setInitiatorId(int initiatorId)
{
    this->initiatorId = initiatorId;
}

int ChordAppMsg::getSubtaskId() const
{
    return this->subtaskId;
}

void ChordAppMsg::setSubtaskId(int subtaskId)
{
    this->subtaskId = subtaskId;
}

size_t ChordAppMsg::getDataArraySize() const
{
    return data_arraysize;
}

int ChordAppMsg::getData(size_t k) const
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    return this->data[k];
}

void ChordAppMsg::setDataArraySize(size_t newSize)
{
    int *data2 = (newSize==0) ? nullptr : new int[newSize];
    size_t minSize = data_arraysize < newSize ? data_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        data2[i] = this->data[i];
    for (size_t i = minSize; i < newSize; i++)
        data2[i] = 0;
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

void ChordAppMsg::setData(size_t k, int data)
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    this->data[k] = data;
}

void ChordAppMsg::insertData(size_t k, int data)
{
    if (k > data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    size_t newSize = data_arraysize + 1;
    int *data2 = new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        data2[i] = this->data[i];
    data2[k] = data;
    for (i = k + 1; i < newSize; i++)
        data2[i] = this->data[i-1];
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

void ChordAppMsg::appendData(int data)
{
    insertData(data_arraysize, data);
}

void ChordAppMsg::eraseData(size_t k)
{
    if (k >= data_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)data_arraysize, (unsigned long)k);
    size_t newSize = data_arraysize - 1;
    int *data2 = (newSize == 0) ? nullptr : new int[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        data2[i] = this->data[i];
    for (i = k; i < newSize; i++)
        data2[i] = this->data[i+1];
    delete [] this->data;
    this->data = data2;
    data_arraysize = newSize;
}

int ChordAppMsg::getMaxResult() const
{
    return this->maxResult;
}

void ChordAppMsg::setMaxResult(int maxResult)
{
    this->maxResult = maxResult;
}

int ChordAppMsg::getGossipOriginIP() const
{
    return this->gossipOriginIP;
}

void ChordAppMsg::setGossipOriginIP(int gossipOriginIP)
{
    this->gossipOriginIP = gossipOriginIP;
}

double ChordAppMsg::getGossipTimestamp() const
{
    return this->gossipTimestamp;
}

void ChordAppMsg::setGossipTimestamp(double gossipTimestamp)
{
    this->gossipTimestamp = gossipTimestamp;
}

int ChordAppMsg::getGossipClientId() const
{
    return this->gossipClientId;
}

void ChordAppMsg::setGossipClientId(int gossipClientId)
{
    this->gossipClientId = gossipClientId;
}

class ChordAppMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_msgType,
        FIELD_sourceId,
        FIELD_destId,
        FIELD_initiatorId,
        FIELD_subtaskId,
        FIELD_data,
        FIELD_maxResult,
        FIELD_gossipOriginIP,
        FIELD_gossipTimestamp,
        FIELD_gossipClientId,
    };
  public:
    ChordAppMsgDescriptor();
    virtual ~ChordAppMsgDescriptor();

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

Register_ClassDescriptor(ChordAppMsgDescriptor)

ChordAppMsgDescriptor::ChordAppMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(chord::ChordAppMsg)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

ChordAppMsgDescriptor::~ChordAppMsgDescriptor()
{
    delete[] propertyNames;
}

bool ChordAppMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ChordAppMsg *>(obj)!=nullptr;
}

const char **ChordAppMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ChordAppMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ChordAppMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 10+base->getFieldCount() : 10;
}

unsigned int ChordAppMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_msgType
        FD_ISEDITABLE,    // FIELD_sourceId
        FD_ISEDITABLE,    // FIELD_destId
        FD_ISEDITABLE,    // FIELD_initiatorId
        FD_ISEDITABLE,    // FIELD_subtaskId
        FD_ISARRAY | FD_ISEDITABLE | FD_ISRESIZABLE,    // FIELD_data
        FD_ISEDITABLE,    // FIELD_maxResult
        FD_ISEDITABLE,    // FIELD_gossipOriginIP
        FD_ISEDITABLE,    // FIELD_gossipTimestamp
        FD_ISEDITABLE,    // FIELD_gossipClientId
    };
    return (field >= 0 && field < 10) ? fieldTypeFlags[field] : 0;
}

const char *ChordAppMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgType",
        "sourceId",
        "destId",
        "initiatorId",
        "subtaskId",
        "data",
        "maxResult",
        "gossipOriginIP",
        "gossipTimestamp",
        "gossipClientId",
    };
    return (field >= 0 && field < 10) ? fieldNames[field] : nullptr;
}

int ChordAppMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "msgType") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "sourceId") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "destId") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "initiatorId") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "subtaskId") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "data") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "maxResult") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "gossipOriginIP") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "gossipTimestamp") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "gossipClientId") == 0) return baseIndex + 9;
    return base ? base->findField(fieldName) : -1;
}

const char *ChordAppMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_msgType
        "int",    // FIELD_sourceId
        "int",    // FIELD_destId
        "int",    // FIELD_initiatorId
        "int",    // FIELD_subtaskId
        "int",    // FIELD_data
        "int",    // FIELD_maxResult
        "int",    // FIELD_gossipOriginIP
        "double",    // FIELD_gossipTimestamp
        "int",    // FIELD_gossipClientId
    };
    return (field >= 0 && field < 10) ? fieldTypeStrings[field] : nullptr;
}

const char **ChordAppMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_msgType: {
            static const char *names[] = { "enum", "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *ChordAppMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
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
            if (!strcmp(propertyName, "enum")) return "chord::MsgType";
            return nullptr;
        default: return nullptr;
    }
}

int ChordAppMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        case FIELD_data: return pp->getDataArraySize();
        default: return 0;
    }
}

void ChordAppMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        case FIELD_data: pp->setDataArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ChordAppMsg'", field);
    }
}

const char *ChordAppMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ChordAppMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return enum2string(pp->getMsgType(), "chord::MsgType");
        case FIELD_sourceId: return long2string(pp->getSourceId());
        case FIELD_destId: return long2string(pp->getDestId());
        case FIELD_initiatorId: return long2string(pp->getInitiatorId());
        case FIELD_subtaskId: return long2string(pp->getSubtaskId());
        case FIELD_data: return long2string(pp->getData(i));
        case FIELD_maxResult: return long2string(pp->getMaxResult());
        case FIELD_gossipOriginIP: return long2string(pp->getGossipOriginIP());
        case FIELD_gossipTimestamp: return double2string(pp->getGossipTimestamp());
        case FIELD_gossipClientId: return long2string(pp->getGossipClientId());
        default: return "";
    }
}

void ChordAppMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType((chord::MsgType)string2enum(value, "chord::MsgType")); break;
        case FIELD_sourceId: pp->setSourceId(string2long(value)); break;
        case FIELD_destId: pp->setDestId(string2long(value)); break;
        case FIELD_initiatorId: pp->setInitiatorId(string2long(value)); break;
        case FIELD_subtaskId: pp->setSubtaskId(string2long(value)); break;
        case FIELD_data: pp->setData(i,string2long(value)); break;
        case FIELD_maxResult: pp->setMaxResult(string2long(value)); break;
        case FIELD_gossipOriginIP: pp->setGossipOriginIP(string2long(value)); break;
        case FIELD_gossipTimestamp: pp->setGossipTimestamp(string2double(value)); break;
        case FIELD_gossipClientId: pp->setGossipClientId(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ChordAppMsg'", field);
    }
}

omnetpp::cValue ChordAppMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: return pp->getMsgType();
        case FIELD_sourceId: return pp->getSourceId();
        case FIELD_destId: return pp->getDestId();
        case FIELD_initiatorId: return pp->getInitiatorId();
        case FIELD_subtaskId: return pp->getSubtaskId();
        case FIELD_data: return pp->getData(i);
        case FIELD_maxResult: return pp->getMaxResult();
        case FIELD_gossipOriginIP: return pp->getGossipOriginIP();
        case FIELD_gossipTimestamp: return pp->getGossipTimestamp();
        case FIELD_gossipClientId: return pp->getGossipClientId();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ChordAppMsg' as cValue -- field index out of range?", field);
    }
}

void ChordAppMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        case FIELD_msgType: pp->setMsgType((chord::MsgType)value.intValue()); break;
        case FIELD_sourceId: pp->setSourceId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_destId: pp->setDestId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_initiatorId: pp->setInitiatorId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_subtaskId: pp->setSubtaskId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_data: pp->setData(i,omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_maxResult: pp->setMaxResult(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_gossipOriginIP: pp->setGossipOriginIP(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_gossipTimestamp: pp->setGossipTimestamp(value.doubleValue()); break;
        case FIELD_gossipClientId: pp->setGossipClientId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ChordAppMsg'", field);
    }
}

const char *ChordAppMsgDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr ChordAppMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ChordAppMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ChordAppMsg *pp = omnetpp::fromAnyPtr<ChordAppMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ChordAppMsg'", field);
    }
}

}  // namespace chord

namespace omnetpp {

}  // namespace omnetpp

