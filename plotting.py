import pandas as pd
import plotly.express as px


def load_data(filepath):
    data = pd.read_csv(filepath, header=None)
    data.columns = ["m", "n", "p", "instance",
                    "solver", "upper", "lower", "time"]
    return data


def list_to_latex_table(data):
    num_rows = len(data)
    num_cols = len(data[0])

    table = "\\begin{tabular}{|" + "c|" * num_cols + "}\n"
    table += "\\hline\n"

    for row in data:
        table += " & ".join(map(str, row)) + " \\\\\n"
        table += "\\hline\n"

    table += "\\end{tabular}"

    return table


def save_table_to_file(data, path):
    with open(path, "w") as f:
        f.write(list_to_latex_table(data))


def shifted_geo_mean(data, shift=1):
    import numpy as np
    return np.exp(np.mean(np.log(data + shift))) - shift


def aggregated_results_table():
    df = load_data("results.csv")

    print(df.nodes.min())
    print(df.nodes.max())

    solvers = list(set(df.solver))
    solvers.sort()  # to get a consistent order

    # solvers = ["greedy", "lazy", "random", "dynamo"]

    timelimits = list(set(df.timelimit))
    timelimits.sort()
    # timelimits = [30]

    for solver in solvers:
        print(solver, "solved", df[df.solver == solver].shape[0])

        # average_cost = df[df.solver == solver].cost.mean()
        # print("average cost", average_cost)

    print("")

    table_results = [["nodes\solvers"] + solvers]

    for timelimit in timelimits:
        for nodes in range(800, 881, 100):
            print("For timelimit", timelimit)
            print("For nodes", nodes)
            df_timelimit = df[df.timelimit == timelimit]
            df_timelimit = df_timelimit[df_timelimit.nodes <= nodes]

            # only consider instances that were solved by all solvers
            for solver in solvers:
                df_timelimit = df_timelimit[df_timelimit.instance.isin(
                    df[df.solver == solver].instance)]

            current_row = [f"n <= {nodes}"]
            for solver in solvers:
                average_cost = df_timelimit[df_timelimit.solver == solver].cost.mean(
                )
                # average_cost = shifted_geo_mean(df_timelimit[df_timelimit.solver == solver].cost)
                # check if nan
                if average_cost != average_cost:
                    rounded = "?"
                else:
                    rounded = round(average_cost)
                current_row.append(rounded)
                print(solver, "average cost", average_cost)
            print("")

            table_results.append(current_row)

    save_table_to_file(table_results, "results.tex")


def all_points_plot(df):

    ps = list(set(df.p))
    ps.sort()

    df["size"] = df["m"] * df["n"]
    print(df.size)

    for p in ps:
        df_timelimit = df[df.p == p]
        fig = px.scatter(
            df_timelimit, x="size", y="time", color="solver",
            hover_data=["instance", "m", "n"], title=f"p={p}"
        )
        # fig.write_image("all_points.png")
        fig.show()


def scatter_plot_compare_two(y_axis="time", y_axis2="time"):
    df = load_data("results.csv")

    df = df[df.time < 3599]
    df = df[df.time >= 0]
    df.upper = df.upper.apply(upper_map)
    df.upper = df.upper.astype(float)

    solver1 = "scip"
    solver2 = "dp"

    data = {
        "x": [], "y": [], "instance": [], "upperscip": [], "upperdp": [], "p": []
    }

    for instance, group in df.groupby(["instance"]):
        if not group[group.solver == solver1].empty and not group[group.solver == solver2].empty:
            upperscip = group[group.solver == "scip"]["upper"].values[0]
            upperdp = group[group.solver == "dp"]["upper"].values[0]

            if abs(upperscip - upperdp) > 0.1:
                continue

            data["x"].append(max(1, group[group.solver == solver1][y_axis].values[0]))
            data["y"].append(
                max(1, group[group.solver == solver2][y_axis2].values[0]))
            data["upperdp"].append(upperdp)
            data["upperscip"].append(upperscip)
            data["instance"].append(instance)
            data["p"].append(int(instance[0].split("_")[-1]))


    log = y_axis == "time"
    fig = px.scatter(data, x="x", y="y",
                     color="p",
                     # marginal_x="rug",
                     # marginal_y="rug",
                     hover_data=["x", "y", "instance", "upperdp", "upperscip"], log_x=log, log_y=log,
                     trendline="ols", trendline_scope="overall",
                     trendline_options=dict(log_x=log, log_y=log),
                     labels={
                         "x": f"{solver1} {y_axis}",
                         "y": f"{solver2} {y_axis}",
                     },
                     )
    line_end = max(0, max(data["y"]))

    import plotly.graph_objects as go
    x = list(range(int(line_end.item())))
    x[-1] = line_end
    fig.add_trace(go.Scatter(x=x, y=x))
    fig.show()


if __name__ == "__main__":
    # aggregated_results_table()

    df = load_data("results.csv")

    instances = set(df.instance)

    df = df[~((df.solver == "dp") & (df.time <= 0) & (df.upper == "-"))]
    solved_by_all = []
    for instance in instances:
        if len(df[df.instance == instance]) == 2:
            solved_by_all.append(instance)

    df = df[df.instance.isin(solved_by_all)]

    def upper_map(val):
        if val == "-":
            return 0
        else:
            return val

    df.upper = df.upper.apply(upper_map)
    df.upper = df.upper.astype(float)

    for solver in df.solver.unique():
        print(f"solver {solver} solved {len(df[(df.solver == solver) & (df.upper > 0)])} instances")
        
    # all_points_plot()
    # scatter_plot_compare_two()
