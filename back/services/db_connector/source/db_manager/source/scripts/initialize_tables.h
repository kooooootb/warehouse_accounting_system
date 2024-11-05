#ifndef H_20AD5E28_4C5C_403F_B71A_06F1CF4722BD
#define H_20AD5E28_4C5C_403F_B71A_06F1CF4722BD

#include <string_view>

namespace srv
{
namespace db
{
namespace dbmgr
{
namespace scpts
{

constexpr std::string_view InitializeTables = R"(
SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

DROP TABLE IF EXISTS public."Report";
DROP TABLE IF EXISTS public."Operation";
DROP TABLE IF EXISTS public."Invoice_Item";
DROP TABLE IF EXISTS public."Invoice";
DROP TABLE IF EXISTS public."Warehouse_Item";
DROP TABLE IF EXISTS public."Warehouse";
DROP TABLE IF EXISTS public."Product";
DROP TABLE IF EXISTS public."Color";
DROP TABLE IF EXISTS public."User";

CREATE TABLE public."Color" (
    color_value integer NOT NULL,
    name text
);

CREATE TABLE public."Invoice" (
    invoice_id bigint NOT NULL,
    name text NOT NULL,
    description text,
    created_date timestamp without time zone NOT NULL,
    created_by bigint NOT NULL,
    company_name text,
    warehouse_to_id bigint,
    warehouse_from_id bigint
);

CREATE TABLE public."Invoice_Item" (
    invoice_id bigint NOT NULL,
    product_id bigint NOT NULL,
    count integer NOT NULL
);

CREATE TABLE public."Operation" (
    operation_id bigint NOT NULL,
    invoice_id bigint NOT NULL,
    product_id bigint NOT NULL,
    warehouse_from_id bigint,
    warehouse_to_id bigint,
    count integer NOT NULL,
    created_date timestamp without time zone NOT NULL
);

CREATE TABLE public."Product" (
    product_id bigint NOT NULL,
    name text NOT NULL,
    pretty_name text,
    description text,
    created_date timestamp without time zone NOT NULL,
    created_by bigint NOT NULL,
    main_color integer
);

CREATE TABLE public."Report" (
    report_id bigint NOT NULL,
    name text NOT NULL,
    report_type integer NOT NULL,
    inner_filename text NOT NULL,
    created_date timestamp without time zone NOT NULL,
    warehouse_id bigint NOT NULL
);

CREATE TABLE public."User" (
    user_id bigint NOT NULL,
    login text NOT NULL,
    password_hashed text NOT NULL,
    name text,
    created_date timestamp with time zone NOT NULL,
    created_by bigint
);

ALTER TABLE public."User" ALTER COLUMN user_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public."User_user_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);

CREATE TABLE public."Warehouse" (
    warehouse_id bigint NOT NULL,
    name text NOT NULL,
    pretty_name text,
    description text,
    location text,
    created_date timestamp without time zone,
    created_by bigint NOT NULL
);

CREATE TABLE public."Warehouse_Item" (
    warehouse_id bigint NOT NULL,
    product_id bigint NOT NULL,
    count integer NOT NULL
);

ALTER TABLE public."Warehouse" ALTER COLUMN warehouse_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public."Warehouse_warehouse_id_seq"
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);

ALTER TABLE ONLY public."Color"
    ADD CONSTRAINT "Color_pkey" PRIMARY KEY (color_value);

ALTER TABLE ONLY public."Invoice_Item"
    ADD CONSTRAINT "Invoice_Item_pkey" PRIMARY KEY (invoice_id);

ALTER TABLE ONLY public."Invoice"
    ADD CONSTRAINT "Invoice_pkey" PRIMARY KEY (invoice_id);

ALTER TABLE ONLY public."Operation"
    ADD CONSTRAINT "Operation_pkey" PRIMARY KEY (operation_id);

ALTER TABLE ONLY public."Product"
    ADD CONSTRAINT "Product_pkey" PRIMARY KEY (product_id);

ALTER TABLE ONLY public."Report"
    ADD CONSTRAINT "Report_pkey" PRIMARY KEY (report_id);

ALTER TABLE ONLY public."User"
    ADD CONSTRAINT "User_pkey" PRIMARY KEY (user_id);

ALTER TABLE ONLY public."Warehouse_Item"
    ADD CONSTRAINT "Warehouse_Item_pkey" PRIMARY KEY (warehouse_id, product_id);

ALTER TABLE ONLY public."Warehouse"
    ADD CONSTRAINT "Warehouse_pkey" PRIMARY KEY (warehouse_id);

ALTER TABLE ONLY public."Invoice_Item"
    ADD CONSTRAINT "Invoice_Item_product_id_fkey" FOREIGN KEY (product_id) REFERENCES public."Product"(product_id);

ALTER TABLE ONLY public."Invoice"
    ADD CONSTRAINT "Invoice_created_by_fkey" FOREIGN KEY (created_by) REFERENCES public."User"(user_id);

ALTER TABLE ONLY public."Invoice"
    ADD CONSTRAINT "Invoice_warehouse_from_id_fkey" FOREIGN KEY (warehouse_from_id) REFERENCES public."Warehouse"(warehouse_id) NOT VALID;

ALTER TABLE ONLY public."Invoice"
    ADD CONSTRAINT "Invoice_warehouse_to_id_fkey" FOREIGN KEY (warehouse_to_id) REFERENCES public."Warehouse"(warehouse_id) NOT VALID;

ALTER TABLE ONLY public."Operation"
    ADD CONSTRAINT "Operation_invoice_id_fkey" FOREIGN KEY (invoice_id) REFERENCES public."Invoice"(invoice_id);

ALTER TABLE ONLY public."Operation"
    ADD CONSTRAINT "Operation_product_id_fkey" FOREIGN KEY (product_id) REFERENCES public."Product"(product_id);

ALTER TABLE ONLY public."Operation"
    ADD CONSTRAINT "Operation_warehouse_from_id_fkey" FOREIGN KEY (warehouse_from_id) REFERENCES public."Warehouse"(warehouse_id);

ALTER TABLE ONLY public."Operation"
    ADD CONSTRAINT "Operation_warehouse_to_id_fkey" FOREIGN KEY (warehouse_to_id) REFERENCES public."Warehouse"(warehouse_id);

ALTER TABLE ONLY public."Product"
    ADD CONSTRAINT "Product_created_by_fkey" FOREIGN KEY (created_by) REFERENCES public."User"(user_id);

ALTER TABLE ONLY public."Product"
    ADD CONSTRAINT "Product_main_color_fkey" FOREIGN KEY (main_color) REFERENCES public."Color"(color_value);

ALTER TABLE ONLY public."Report"
    ADD CONSTRAINT "Report_warehouse_id_fkey" FOREIGN KEY (warehouse_id) REFERENCES public."Warehouse"(warehouse_id);

ALTER TABLE ONLY public."User"
    ADD CONSTRAINT "User_created_by_fkey" FOREIGN KEY (created_by) REFERENCES public."User"(user_id) NOT VALID;

ALTER TABLE ONLY public."Warehouse_Item"
    ADD CONSTRAINT "Warehouse_Item_product_id_fkey" FOREIGN KEY (product_id) REFERENCES public."Product"(product_id);

ALTER TABLE ONLY public."Warehouse_Item"
    ADD CONSTRAINT "Warehouse_Item_warehouse_id_fkey" FOREIGN KEY (warehouse_id) REFERENCES public."Warehouse"(warehouse_id);

ALTER TABLE ONLY public."Warehouse"
    ADD CONSTRAINT "Warehouse_created_by_fkey" FOREIGN KEY (created_by) REFERENCES public."User"(user_id);

INSERT INTO "User" (login, password_hashed, name, created_date, created_by) VALUES('superuser', 'C191615151114051D19181D1D1E151F11171918131C1B18181F1F1A141819181', 'Super User', current_timestamp, NULL);
)";

}
}  // namespace dbmgr
}  // namespace db
}  // namespace srv

#endif  // H_20AD5E28_4C5C_403F_B71A_06F1CF4722BD