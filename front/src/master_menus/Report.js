import React from "react";
import axios from "axios";

import styles from "./Report.module.css";

export default class ReportMaster extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            type: "current",
            reports: [],
            warehouses: [],
            reportSize: null,
            reportsBatch: 5,
            currentReport: {},
            byPeriodReport: {},
        };
    }

    componentDidMount() {
        this.loadReportSize();
        this.loadWarehouses();
    }

    loadReportSize() {
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

    loadWarehouses() {
        axios.get("/api/warehouses", {
            headers: {
                authorization: this.props.user.token
            },
            params: {
                limit: 0,
                offset: 0
            }
        })
            .then((response) => {
                const warehouses = response.data.result.map(({ warehouse_id, pretty_name }) => ({ warehouse_id, pretty_name }));
                this.setState({ warehouses });
            })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });
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
        if (rep?.name == null || rep?.warehouse_id == null) return false

        // passed
        return true;
    }

    isByPeriodReportGood(rep) {
        // check name
        if (rep?.name == null || rep?.period_from == null || rep?.period_to == null || rep?.warehouse_id == null) return false

        // passed
        return true;
    }

    saveCurrentReport() {
        if (!this.isCurrentReportGood(this.state.currentReport)) {
            alert("Required fields not filled")
            return;
        }

        axios({
            url: "/api/reports/current",
            method: "POST",
            headers: {
                authorization: this.props.user.token
            },
            data: { ...this.state.currentReport }
        })
            .catch((error) => {
                if (error.status === 401) {
                    this.props.setNextRedirect("/");
                    this.setState({ redirectTo: "/login" });
                }
                console.error(error);
            });

        this.setState({ currentReport: {} });
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
        return (
            <div className={styles.reportForm}>
                <div className={styles.inputGroup}>
                    <label>Report name:</label>
                    <input type="text" placeholder="Name" value={this.state.currentReport?.name ?? ""} onChange={(event) => this.setState({ currentReport: { ...this.state.currentReport, name: event.target.value } })} />
                </div>
                <div className={styles.textareaGroup}>
                    <label>Report description:</label>
                    <textarea type="text" placeholder="Description" value={this.state.currentReport?.description ?? ""} onChange={(event) => this.setState({ currentReport: { ...this.state.currentReport, description: event.target.value } })} />
                </div>
                <div className={styles.warehousesGroup}>
                    <div className={styles.warehouseSelect}>
                        <label>For warehouse: </label>
                        <select value={this.state.currentReport.warehouse_id} onChange={(event) => { this.setState({ currentReport: { ...this.state.currentReport, warehouse_id: event.target.value === "" ? undefined : Number(event.target.value) } }) }}>
                            <option value={""}>choose warehouse</option>
                            {this.state.warehouses.map((wh) => (
                                <option key={wh.warehouse_id} value={wh.warehouse_id}>
                                    {wh.pretty_name}
                                </option>
                            ))}
                        </select>
                    </div>
                </div>
                <div className={styles.submitButton}>
                    <button type="submit" onClick={() => this.saveCurrentReport()}>Submit</button>
                </div>
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