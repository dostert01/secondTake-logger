#include "apistructs.h"
#include <iostream>

/*
    BinaryProcessingData
*/
BinaryProcessingData::~BinaryProcessingData() {}

/*
    PipelineStepInitData
*/
std::optional<std::string> PipelineStepInitData::getNamedArgument(const std::string& argumentName) {
    auto search = namedArguments.find(argumentName);
    if(search != namedArguments.end()) {
        return search->second;
    }
    return std::nullopt;
}

/*
    ProcessingPayload
*/
ProcessingPayload::ProcessingPayload(std::string mimetype, std::string payload) {
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::ProcessingPayload(std::string mimetype, std::shared_ptr<BinaryProcessingData> payload){
    setMimeType(mimetype);
    setPayload(payload);
}

ProcessingPayload::~ProcessingPayload() {}

std::string ProcessingPayload::payloadAsString() {
    return stringPayloadData;
}

std::shared_ptr<BinaryProcessingData> ProcessingPayload::payloadAsBinaryData() {
    return binaryPayloadData;
}

void ProcessingPayload::setMimeType(std::string mimetype) {
    this->mimetype = mimetype;
}

void ProcessingPayload::setPayload(std::string payload) {
    this->stringPayloadData = payload;
}

void ProcessingPayload::setPayload(std::shared_ptr<BinaryProcessingData> payload) {
    this->binaryPayloadData = payload;
}

/*
    PipelineProcessingData
*/
PipelineProcessingData::~PipelineProcessingData() {
    /*
    std::multimap<std::string, std::shared_ptr<ProcessingPayload>>::iterator it;
    for(it = namedPayloadData.begin(); it != namedPayloadData.end(); ++it) {
        if(it->second != NULL) {
            std::cout << "deleting payload of name: " << it->first << std::endl;
            delete it->second;
        }
    }
    */
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, std::string data) {
    std::shared_ptr<ProcessingPayload> payload = std::make_shared<ProcessingPayload>(mimetype, data);
    namedPayloadData.insert(std::make_pair(payloadName, std::move(payload)));
}

void PipelineProcessingData::addPayloadData(std::string payloadName, std::string mimetype, std::shared_ptr<BinaryProcessingData> data) {
    std::shared_ptr<ProcessingPayload> payload = std::make_shared<ProcessingPayload>(mimetype, data);
    namedPayloadData.insert(std::make_pair(payloadName, std::move(payload)));
}

void PipelineProcessingData::addError(std::string errorCode, std::string errorMessage) {
    std::shared_ptr<BinaryProcessingData> error = std::make_shared<ProcessingError>(errorCode, errorMessage);
    addPayloadData(PAYLOAD_NAME_PROCESSING_ERROR, PAYLOAD_MIMETYPE_APPLICATION_OCTET_STREAM, error);
}

bool PipelineProcessingData::hasError() {
    return namedPayloadData.contains(PAYLOAD_NAME_PROCESSING_ERROR);
}

std::vector<ProcessingError> PipelineProcessingData::getAllErrors() {
    std::vector<ProcessingError> returnVector;
    if(hasError()) {
        auto allErrors = namedPayloadData.equal_range(PAYLOAD_NAME_PROCESSING_ERROR);
        for(auto iterator = allErrors.first; iterator != allErrors.second; ++iterator) {
            ProcessingError* error = (ProcessingError*)(iterator->second.get()->payloadAsBinaryData().get());
            returnVector.emplace_back(error);
        }
    }
    return returnVector;
}

std::optional<std::shared_ptr<ProcessingPayload>> PipelineProcessingData::getPayload(std::string payloadName) {
    auto search = namedPayloadData.find(payloadName);
    if(search != namedPayloadData.end()) {
        return search->second;
    }
    return std::nullopt;
}

uint PipelineProcessingData::getCountOfPayloads() {
    return namedPayloadData.size();
}

uint PipelineProcessingData::getProcessingCounter() {
    return processingCounter;
}

void PipelineProcessingData::increaseProcessingCounter() {
    processingCounter++;
}

void PipelineProcessingData::setLastProcessedPipelineName(std::string pipelineName) {
    lastProcessedPipelineName = pipelineName;
}

std::string PipelineProcessingData::getLastProcessedPipelineName() {
    return lastProcessedPipelineName;
}

/*
    ProcessingError
*/
ProcessingError::ProcessingError(std::string errorCode, std::string errorMessage) : BinaryProcessingData() {
    this->errorCode = errorCode;
    this->errorMessage = errorMessage;
}

ProcessingError::ProcessingError(ProcessingError* other) : BinaryProcessingData() {
    errorCode = other->getErrorCode();
    errorMessage = other->getErrorMessage();
}

std::string ProcessingError::getErrorCode() {
    return errorCode;
}

std::string ProcessingError::getErrorMessage() {
    return errorMessage;
}
