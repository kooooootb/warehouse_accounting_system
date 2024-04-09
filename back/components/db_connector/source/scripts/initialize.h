#ifndef H_0BF1B0C4_FD49_453B_8D4F_61DE4088373A
#define H_0BF1B0C4_FD49_453B_8D4F_61DE4088373A

#include <string_view>

namespace db
{

constexpr std::string_view INITIALIZE_DB = R"(
create table if not exists Section (
	section_id serial PRIMARY KEY,
	sec_spec_num int NOT NULL,
	SecName text NOT NULL
);

create table if not exists Roles (
	role_id serial PRIMARY KEY,
	role_name varchar(255) NOT NULL
);

create table if not exists Person (
	person_id serial NOT NULL PRIMARY KEY,
	first_name text NOT NULL,
	last_name text NOT NULL,
	birthday date,
	login text NOT NULL,
	per_password text NOT NULL,
	per_role int NOT NULL,
	FOREIGN KEY (per_role) REFERENCES Roles (role_id)
	ON DELETE RESTRICT ON UPDATE CASCADE
);

create table if not exists SpecificationStatus (
	spec_status_id int NOT NULL PRIMARY KEY,
	spec_status_name text
);

create table if not exists Project (
	project_id serial NOT NULL PRIMARY KEY,
	project_name text NOT NULL,
	proj_release date
);

create table if not exists Specification (
	specification_id serial NOT NULL PRIMARY KEY,
	spec_name text NOT NULL,
	analytic int NOT NULL,
	status_spec int NOT NULL,
	project int,
	FOREIGN KEY (analytic) REFERENCES Person (person_id),
	FOREIGN KEY (status_spec) REFERENCES SpecificationStatus (spec_status_id),
	FOREIGN KEY (project) REFERENCES Project (project_id)
);

create table if not exists RequirementType (
	ReqType_id int NOT NULL PRIMARY KEY,
	type_name text
);

create table if not exists RequirementStatus (
	req_status_id int NOT NULL PRIMARY KEY,
	req_status_name text
);

create table if not exists Requirement (
	requirement_id serial NOT NULL PRIMARY KEY,
	type_id int NOT NULL,
	in_specification int,
	description text,
	status_req int NOT NULL,
	project int,
	req_release date,
	FOREIGN KEY (type_id) REFERENCES RequirementType (ReqType_id),
	FOREIGN KEY (in_specification) REFERENCES Specification (specification_id),
	FOREIGN KEY (status_req) REFERENCES RequirementStatus (req_status_id),
	FOREIGN KEY (project) REFERENCES Project (project_id)
);

create table if not exists TypesRelation (
	rel_type_id int NOT NULL PRIMARY KEY,
	rel_type_name text NOT NULL
);

create table if not exists RelatedRequirements (
	main_req int NOT NULL,
	depended_req int NOT NULL,
	relation_type int NOT NULL,
	FOREIGN KEY (main_req) REFERENCES Requirement (requirement_id),
	FOREIGN KEY (depended_req) REFERENCES Requirement (requirement_id),
	FOREIGN KEY (relation_type) REFERENCES TypesRelation (rel_type_id),
	PRIMARY KEY (main_req, depended_req)
);

create table if not exists Req_in_Section_in_Spec (
	RSS_id serial NOT NULL PRIMARY KEY,
	spec_id int NOT NULL,
	requirement_id int NOT NULL,
	section_id int NOT NULL,
	req_sec_num int,
	FOREIGN KEY (spec_id) REFERENCES Specification (specification_id),
	FOREIGN KEY (requirement_id) REFERENCES Requirement (requirement_id),
	FOREIGN KEY (section_id) REFERENCES Section (section_id),
	CONSTRAINT RSS_Info UNIQUE(spec_id, requirement_id, section_id)
);

create table if not exists Approve (
	approve_id serial NOT NULL PRIMARY KEY,
	reviewer_id int,
	rss_id int NOT NULL,
	Ok boolean,
	FOREIGN KEY (rss_id) REFERENCES Req_in_Section_in_Spec (RSS_id),
	FOREIGN KEY (reviewer_id) REFERENCES Person (person_id)
);

create table if not exists Comment (
	decision_id serial NOT NULL PRIMARY KEY,
	to_approve_id int,
	date_com date,
	decision_name text,
	FOREIGN KEY (to_approve_id) REFERENCES Approve (approve_id)
);

create table if not exists ExternalSystems (
	test_man_sys text,
	proj_man_sys text
);

INSERT INTO RequirementType (ReqType_id, type_name) VALUES
(1, 'functional'),
(2, 'nonfunctional');

INSERT INTO RequirementStatus (req_status_id, req_status_name) VALUES
(1, 'new'),
(2, 'inspecification'),
(3, 'negotiation'),
(4, 'approved'),
(5, 'developed'),
(6, 'supported'),
(7, 'deleted');

INSERT INTO SpecificationStatus (spec_status_id, spec_status_name) VALUES
(1, 'approved'),
(2, 'draft'),
(3, 'obsolete');

INSERT INTO TypesRelation (rel_type_id, rel_type_name) VALUES
(1, 'hierarchy'),
(2, 'horizontal'),
(3, 'copied');

INSERT INTO Roles (role_id, role_name) VALUES
(1, 'admin'),
(2, 'analytic'),
(3, 'reviewer');

INSERT INTO Person (first_name, last_name, login, per_password, per_role) VALUES
('admin', 'admin', 'admin', 'admin', 1);
)";

}  // namespace db

#endif  // H_0BF1B0C4_FD49_453B_8D4F_61DE4088373A