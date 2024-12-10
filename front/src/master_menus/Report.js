import React from "react";
import axios from "axios";

import styles from "./Report.module.css";

export default class ReportMaster extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            type: "current",
            reports: [],
            reportSize: null,
            reportsBatch: 5,
            currentReport: {},
            byPeriodReport: {},
        };
    }

    componentDidMount() {
        if (this.state.reportSize == null) {
            axios.get("/api/reportsize", {
                headers: {
                    authorization: this.props.user.token
                },
            })
                .then((response) => {
                    const reportSize = response.data.count;
                    this.setState({ reportSize }, () => this.loadNextReports());
                })
                .catch((error) => {
                    if (error.status === 401) {
                        this.props.setNextRedirect("/");
                        this.setState({ redirectTo: "/login" });
                    }
                    console.error(error);
                });
        }

    }

    loadNextReports() {
        if (this.state.reports.length >= this.state.reportSize) {
            console.error("tried to load more reports than available");
        }

        axios.get("/api/reports", {
            headers: {
                authorization: this.props.user.token
            },
            params: {
                limit: this.state.reportsBatch,
                offset: this.state.reports.length
            }
        })
            .then((response) => {
                const reports = response.data.result;
                this.setState({ reports: [...this.state.reports, ...reports] });
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    selectType(newType) {
        this.setState({ type: newType });
    }

    isCurrentReportGood(rep) {
        // check name
        if (rep?.name != null && rep?.warehouse_id != null) return false

        // passed
        return true;
    }

    isByPeriodReportGood(rep) {
        // check name
        if (rep?.name != null && rep?.period_from != null && rep?.period_to != null && rep?.warehouse_id != null) return false

        // passed
        return true;
    }

    saveInvoice() {
        if (!this.isInvoiceGood(this.state.invoice)) {
            alert("Required fields not filled")
            return;
        }

        const products = this.state.products.map((pr) => {
            if (pr?.product_id != null) {
                return { id: pr.product_id, count: pr.count }
            }
            else {
                return pr
            }
        })

        let data = {
            invoice_name: this.state.invoice.invoice_name,
            ...(this.state.invoice.invoice_description && { invoice_description: this.state.invoice.invoice_description }),
            products
        }
        let url = ""

        switch (this.state.type) {
            case "new":
                data = { ...data, warehouse_to: this.state.warehouse_to_id }
                url = "/api/products/create"
                break
            case "move":
                data = { ...data, warehouse_to: this.state.warehouse_to_id, warehouse_from: this.state.warehouse_from_id }
                url = "/api/products/move"
                break
            case "remove":
                data = { ...data, warehouse_from: this.state.warehouse_from_id }
                url = "/api/products/remove"
                break
            default: console.error(`wrong type: ${this.state.type}`)
        }

        axios({
            url,
            method: "POST",
            headers: {
                authorization: this.props.user.token
            },
            data
        })
            .then((response) => {
                this.props.closeMaster();
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/warehouses");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
    }

    renderSwitch() {
        switch (this.state.type) {
            case "list": return this.renderList();
            case "current": return this.renderCurrent();
            case "by_period": return this.renderByPeriod();
            default: console.error(`Incorrect type: ${this.state.type}`);
        }
    }

    renderList() {
        return (
            <div className={styles.reportList}>
                {this.state.reports.map((rep) => (
                    <div key={rep.report_id} className={styles.reportEntry}>
                        <div className={styles.reportBriefInfo}>
                            <p>name: {rep.name}</p>
                            <p>report_id: {rep.report_id}</p>
                            <p>type: {rep.report_type}</p>
                            <p>created_by: {rep.created_date}</p>
                            <p>created_date: {rep.created_by}</p>
                        </div>
                        <div className={styles.reportDescription} onClick={() => this.loadReport(rep.filepath)}>
                            <p>description:<br />{rep.description}</p>
                        </div>
                        <a className={styles.reportDownloadButton} href={rep.filepath}>Download report</a>
                    </div>
                ))
                }
                {
                    this.state.reports.length >= this.state.reportSize ? null : <div className={styles.reportLoadMore} onClick={() => this.loadNextReports()}>
                        Load more reports
                    </div>
                }
            </div >
        )
    }

    renderCurrent() {
        return (<div className={styles.invoiceForm}>

        </div>
        )
    }

    renderByPeriod() {

    }

    render() {
        return (
            <div className={styles.mainContent}>
                <div className={styles.typeSwitchHeader}>
                    <div className={this.state.type === "list" ? styles.selectedTypeSwitcher : styles.typeSwitcher} onClick={() => this.selectType("list")}>
                        List reports
                    </div>
                    <div className={this.state.type === "current" ? styles.selectedTypeSwitcher : styles.typeSwitcher} onClick={() => this.selectType("current")}>
                        Create current state report
                    </div>
                    <div className={this.state.type === "by_period" ? styles.selectedTypeSwitcher : styles.typeSwitcher} onClick={() => this.selectType("by_period")}>
                        Create by period report
                    </div>
                </div>
                {this.renderSwitch()}
            </div >
        );
    }
}