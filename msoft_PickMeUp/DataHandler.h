#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <QString>
#include <qpixmap.h>

// Class for handling database data
class DataHandler
{

public:
// Enumeration classes
enum class UserType
{
	Basic,
	FreelanceCourier,
	CompanyCourier,
	Technician,
	Operator
};

//---------------------------------------------------------------------------

enum class ParcelStatus
{
	Created,
	ToBeSent,
	Sent,
	Arrived,
	ERROR
};

//---------------------------------------------------------------------------

enum class ParcelSize
{
	SMALL,
	MEDIUM,
	BIG,
};

//---------------------------------------------------------------------------

enum class TaskStatus
{
	Created,
	Assigned,
	Completed,
	Pending,
	AcceptedReclamation,
	DeniedReclamation
};

//---------------------------------------------------------------------------

enum class TaskType
{
	Delivery,
	Repair,
	Reclamation
};

//---------------------------------------------------------------------------

enum class ReportStatus
{
	Malfunction,
	Working,
	NoReport
};

//---------------------------------------------------------------------------

// Enum to string parcel
static QString parcelStatusToString(ParcelStatus status)
{
	switch (status)
	{
	case ParcelStatus::Created: return "Created";
	case ParcelStatus::ToBeSent: return "ToBeSent";
	case ParcelStatus::Sent: return "Sent";
	case ParcelStatus::Arrived: return "Arrived";
	case ParcelStatus::ERROR: return "ERROR";
	default: return "Unknown";
	}
}

//---------------------------------------------------------------------------

// Enum to string conversions
static QString taskStatusToString(TaskStatus status)
{
	switch (status) {
	case TaskStatus::Created:
		return "Created";
	case TaskStatus::Assigned:
		return "Assigned";
	case TaskStatus::Completed:
		return "Completed";
	case TaskStatus::Pending:
		return "Pending";
	case TaskStatus::AcceptedReclamation:
		return "Accepted Reclamation";
	case TaskStatus::DeniedReclamation:
		return "Denied Reclamation";
	default:
		return "Unknown";
	}
}

//---------------------------------------------------------------------------

static QString taskTypeToString(TaskType type)
{
	switch (type) {
	case TaskType::Delivery:
		return "Delivery";
	case TaskType::Repair:
		return "Repair";
	case TaskType::Reclamation:
		return "Reclamation";
	default:
		return "Unknown";
	}
}

//---------------------------------------------------------------------------

static QString reportStatusToString(ReportStatus status)
{
	switch (status) {
	case ReportStatus::Malfunction:
		return "Malfunction";
	case ReportStatus::Working:
		return "Working";
	case ReportStatus::NoReport:
		return "No report";
	default:
		return "Unknown";
	}
}

//---------------------------------------------------------------------------

//Representation of a user in the database
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

//---------------------------------------------------------------------------

//Representation of a package in the database
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

//---------------------------------------------------------------------------

// Representation of a task in the database
struct Task
{
	Task(int32_t id, int32_t objectId, std::string asignee, std::string creator, std::string location, TaskType taskType)
		:m_id(id), m_objectId(objectId), m_asignee(asignee), m_creator(creator), m_location(location), m_taskType(taskType), m_taskStatus(TaskStatus::Created), m_pixmap(QPixmap{}), m_description(std::string{})
	{}

	Task(int32_t id, int32_t objectId, std::string asignee, std::string creator, std::string location, TaskType taskType, QPixmap pixmap, std::string description)
		:m_id(id), m_objectId(objectId), m_asignee(asignee), m_creator(creator), m_location(location), m_taskType(taskType), m_taskStatus(TaskStatus::Created), m_pixmap(pixmap), m_description(description)
	{}

	int32_t m_id;
	int32_t m_objectId;
	std::string m_asignee;
	std::string m_creator;
	std::string m_location;
	TaskType m_taskType;
	TaskStatus m_taskStatus;
	QPixmap m_pixmap;
	std::string m_description; 
};

//---------------------------------------------------------------------------

// Representation of a report in the database
struct Report
{
	Report(int32_t id, std::string location, time_t reportDate, ReportStatus status)
		:m_id(m_id), m_location(location), m_reportDate(reportDate), m_status(status)
	{}

	int32_t m_id;
	std::string m_location;
	time_t m_reportDate;
	ReportStatus m_status;
};

//---------------------------------------------------------------------------

// Representation of a repair request in the database
struct RepairRequest
{
	RepairRequest(int32_t id, std::string reporterName, std::string severity, std::string description, std::string location)
		:m_id(m_id), m_location(location), m_reporterName(reporterName), m_severity(severity), m_description(description)
	{}

	int32_t m_id;
	std::string m_reporterName;
	std::string m_severity; 
	std::string m_description;
	std::string m_location;
};

//---------------------------------------------------------------------------

public:
	DataHandler(const DataHandler&) = delete;
	DataHandler& operator=(const DataHandler&) = delete;
	// Get the instance of the datahandler (singleton)
	static DataHandler& getInstance()
	{
		static DataHandler instance;
		return instance;
	}

	//---------------------------------------------------------------------------

	// Gets a user from the database if exist and password matches
	User& getUser(const std::string& email, const std::string& password)
	{
		auto it = m_users.find(email);
		if (it != m_users.end() && it->second.m_password == password) {
			return it->second;
		}
		throw std::runtime_error("Invalid email or password");
	}

	//---------------------------------------------------------------------------

	// Writtes a new package to the database
	int32_t writteParcel(const std::string& sender, const std::string& reviever, const std::string& sAddress, const std::string& dAddress)
	{
		int32_t assignedId = m_currentParcelId;
		m_parcels.emplace(assignedId, Parcel{ assignedId , sender, reviever, sAddress,  dAddress});
		m_currentParcelId++;
		return assignedId;
	}

	//---------------------------------------------------------------------------

	// Writtes a new task to the database 
	int32_t writteTask(int32_t objectId, std::string asignee, std::string creator, std::string location, TaskType taskType)
	{
		int32_t assignedId = m_currentTaskId;
		m_tasks.emplace(assignedId, Task{ assignedId , objectId, asignee, creator,  location, taskType });
		m_currentTaskId++;
		return assignedId;
	}

	//---------------------------------------------------------------------------

	// Writtes a new reclamation task to the database 
	int32_t writteTask(int32_t objectId, std::string asignee, std::string creator, std::string location, TaskType taskType, QPixmap image, std::string description)
	{
		int32_t assignedId = m_currentTaskId;
		m_tasks.emplace(assignedId, Task{ assignedId , objectId, asignee, creator,  location, taskType, image,  description});
		m_currentTaskId++;
		return assignedId;
	}

	//---------------------------------------------------------------------------

	// Gets a task from the database, if exists
	Task& getTask(int32_t taskId)
	{
		auto it = m_tasks.find(taskId);
		if (it != m_tasks.end()) {
			return it->second;
		}
		throw std::runtime_error("Task does not exist");

	}

	//---------------------------------------------------------------------------

	// Writtes a new repair request to the database
	int32_t writteRepair(std::string reporterName, std::string severity, std::string description, std::string location)
	{
		int32_t assignedId = m_currentTaskId;
		m_repairRequest.emplace(assignedId, RepairRequest{ assignedId , reporterName, severity, description,  location });
		m_currentRepairId++;
		return assignedId;
	}

	//---------------------------------------------------------------------------

	// Gets a package from the database, if exists
	Parcel& getParcel(int32_t parcelId)
	{
		auto it = m_parcels.find(parcelId);
		if (it != m_parcels.end()) {
			return it->second;
		}
		throw std::runtime_error("Parcel does not exist");
	}

	//---------------------------------------------------------------------------

	// Gets the parcel list from the database
	std::map<int32_t, Parcel> getParcels() { return m_parcels; }

	//---------------------------------------------------------------------------
	
	// Gets the task list from the database
	std::map<int32_t, Task> getTasks() { return m_tasks; }

	//---------------------------------------------------------------------------

	// Gets the report list from the database
	std::map<int32_t, Report> getReports() { return m_reports; }

	//---------------------------------------------------------------------------

private:
	DataHandler()
		:m_currentUserId(0), m_currentParcelId(0), m_currentTaskId(0), m_currentReportId(0), m_currentRepairId(0)
	{
		m_users.emplace("a", User{ 0, "Mr.Test", "a", "a", UserType::Basic });
		m_users.emplace("b", User{ 1, "Mr.Courier", "b", "courier@mail.com", UserType::FreelanceCourier });
		m_users.emplace("c", User{ 2, "Mr.Tech", "c", "operator@mail.com", UserType::Operator });
		m_users.emplace("d", User{ 3, "Mr.Tech", "d", "operator@mail.com", UserType::Technician });

		m_reports.emplace(0, Report{ 0, "Location 0", 0, ReportStatus::Malfunction });
		
		Task filler1 = Task{ m_currentTaskId++, -1,"Darius", "Ado", "Rumunsko", TaskType::Delivery };
		Task filler2 = Task{ m_currentTaskId++, -2,"Dusan", "Ado", "Rumunsko", TaskType::Delivery };
		filler1.m_taskStatus = TaskStatus::Completed;
		filler2.m_taskStatus = TaskStatus::Assigned;
		m_tasks.emplace(filler1.m_id, filler1);
		m_tasks.emplace(filler2.m_id, filler2);
	}

private:
	int32_t m_currentUserId;
	int32_t m_currentParcelId;
	int32_t m_currentTaskId;
	int32_t m_currentReportId;
	int32_t m_currentRepairId;
	std::map<std::string, User> m_users;
	std::map<int32_t, Parcel> m_parcels;
	std::map<int32_t, Task> m_tasks;
	std::map<int32_t, Report> m_reports;
	std::map<int32_t, RepairRequest> m_repairRequest;
};