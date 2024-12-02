#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <QString>

class DataHandler
{

public:

enum class UserType
{
	Basic,
	FreelanceCourier,
	CompanyCourier,
	Technician
};

enum class ParcelStatus
{
	Created,
	ToBeSent,
	Sent,
	Arrived,
	ERROR
};

enum class ParcelSize
{
	SMALL,
	MEDIUM,
	BIG,
};

enum class TaskStatus
{
	Created,
	Assigned
};

enum class TaskType
{
	Delivery,
	Repair,
	Pain
};

static QString taskStatusToString(TaskStatus status)
{
	switch (status) {
	case TaskStatus::Created:
		return "Created";
	case TaskStatus::Assigned:
		return "Assigned";
	default:
		return "Unknown";
	}
}

static QString taskTypeToString(TaskType type)
{
	switch (type) {
	case TaskType::Delivery:
		return "Delivery";
	case TaskType::Repair:
		return "Repair";
	case TaskType::Pain:
		return "Pain";
	default:
		return "Unknown";
	}
}

struct User
{
	User(int32_t id, std::string name, std::string password, std::string email, UserType type)
		:m_userId(id), m_name(name), m_password(password), m_email(email), m_userType(type)
	{}

	uint32_t m_userId;
	std::string m_name;
	std::string m_password;
	std::string m_email;
	UserType m_userType;
};

struct Parcel
{
	Parcel(int32_t id, std::string sender, std::string reviever, std::string sAddress, std::string dAddress)
		:m_parcelId(id), m_sender(sender), m_reciever(reviever), m_sourceAddress(sAddress), m_destinationAddress(dAddress), m_status(ParcelStatus::Created), m_size(ParcelSize::BIG)
	{}

	Parcel(const Parcel& other) = default;
	Parcel& operator=(const Parcel& other) = default;
	Parcel(Parcel&& other) noexcept = default;
	Parcel& operator=(Parcel&& other) noexcept = default;
	ParcelSize getSize() { return m_size; }
	void setState(ParcelStatus status) {
		m_status = status; 
		if (status == ParcelStatus::Sent)
		{
			DataHandler::getInstance().writteTask(m_parcelId, std::string{}, m_sender, m_sourceAddress, TaskType::Delivery);
		}
	}
	uint32_t m_parcelId;
	std::string m_sender;
	std::string m_reciever;
	std::string m_sourceAddress;
	std::string m_destinationAddress;
	ParcelStatus m_status;
	ParcelSize m_size;
};

struct Task
{
	Task(int32_t id, int32_t objectId, std::string asignee, std::string creator, std::string location, TaskType taskType)
		:m_id(m_id), m_objectId(objectId), m_asignee(asignee), m_creator(creator), m_location(location), m_taskType(taskType), m_taskStatus(TaskStatus::Created)
	{}

	int32_t m_id;
	int32_t m_objectId;
	std::string m_asignee;
	std::string m_creator;
	std::string m_location;
	TaskType m_taskType;
	TaskStatus m_taskStatus;
};

public:
	DataHandler(const DataHandler&) = delete;
	DataHandler& operator=(const DataHandler&) = delete;
	
	static DataHandler& getInstance()
	{
		static DataHandler instance;
		return instance;
	}

	User& getUser(const std::string& email, const std::string& password)
	{
		auto it = m_users.find(email);
		if (it != m_users.end() && it->second.m_password == password) {
			return it->second;
		}
		throw std::runtime_error("Invalid email or password");
	}

	int32_t writteParcel(const std::string& sender, const std::string& reviever, const std::string& sAddress, const std::string& dAddress)
	{
		int32_t assignedId = m_currentParcelId;
		m_parcels.emplace(assignedId, Parcel{ m_currentParcelId , sender, reviever, sAddress,  dAddress});
		m_currentParcelId++;
		return assignedId;
	}

	int32_t writteTask(int32_t objectId, std::string asignee, std::string creator, std::string location, TaskType taskType)
	{
		int32_t assignedId = m_currentTaskId;
		m_tasks.emplace(assignedId, Task{ m_currentTaskId , objectId, asignee, creator,  location, taskType });
		m_currentTaskId++;
		return assignedId;
	}

	Parcel& getParcel(int32_t parcelId)
	{
		auto it = m_parcels.find(parcelId);
		if (it != m_parcels.end()) {
			return it->second;
		}
		throw std::runtime_error("Parcel does not exist");
	}

	std::map<int32_t, Task> getTasks() { return m_tasks; }

private:
	DataHandler()
		:m_users(std::map<std::string, User>{}), m_currentUserId(0), m_currentParcelId(0), m_currentTaskId(0)
	{
		m_users.emplace("a", User{ 0, "Mr.Test", "a", "a", UserType::Basic });
		m_users.emplace("b", User{ 1, "Mr.Courier", "b", "courier@mail.com", UserType::FreelanceCourier });
		m_users.emplace("tech@mail.com", User{ 2, "Mr.Tech", "strongpassword", "tech@mail.com", UserType::Technician });
	}

private:
	int32_t m_currentUserId;
	int32_t m_currentParcelId;
	int32_t m_currentTaskId;
	std::map<std::string, User> m_users;
	std::map<int32_t, Parcel> m_parcels;
	std::map<int32_t, Task> m_tasks;
};