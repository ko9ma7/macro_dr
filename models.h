#ifndef MODELS_H
#define MODELS_H
#include "allosteric_models.h"
#include "qmodel.h"
namespace macrodr {





auto model00_7 = Model0::Model([]() {
    auto names_model = std::vector<std::string>{"kon",
                                                "koff",
                                                "gatin_on",
                                                "gating_off",
                                                "inactivation_rate",
                                                "unitary_current"};
    auto names_other = std::vector<std::string>{
                                                "Current_Noise", "Current_Baseline", "Num_ch_mean"};
    
    std::size_t N = 6ul;
    
    auto v_Q0_formula = Q0_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Q0_formula()[1][0] = "koff";
    v_Q0_formula()[2][1] = "2*koff";
    v_Q0_formula()[3][2] = "3*koff";
    v_Q0_formula()[3][4] = "gatin_on";
    v_Q0_formula()[4][3] = "gating_off";
    v_Q0_formula()[0][5] = "inactivation_rate";
    
    auto v_Qa_formula = Qa_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Qa_formula()[0][1] = "3*kon";
    v_Qa_formula()[1][2] = "2*kon";
    v_Qa_formula()[2][3] = "kon";
    auto v_g_formula = g_formula(std::vector<std::string>(N, ""));
    v_g_formula()[4] = "unitary_current";
    
    names_model.insert(names_model.end(), names_other.begin(),
                       names_other.end());
    auto p = Parameters<Model0>(
        std::vector<double>{10, 200, 1500, 50, 1e-5, 1, 1e-4, 1, 1000});
    
    auto logp =
        Parameters<Model0>(apply([](auto x) { return std::log10(x); }, p()));
    
    return std::tuple(
        [](const auto &logp) {
            using std::pow;
            auto p = apply([](const auto &x) { return pow(10.0, x); }, logp());
            auto kon = p[0];
            auto koff = p[1];
            auto gating_on = p[2];
            auto gating_off = p[3];
            auto inactivation_rate = p[4];
            auto v_unitary_current = p[5] * -1.0;
            auto Npar = 6ul;
            auto v_curr_noise = p[Npar];
            auto v_baseline = logp()[Npar + 1];
            //  auto v_Num_ch_mean=p[Npar+2];
            //  auto v_std_log_Num_ch=p[Npar+3];
            
            auto Npar2 = Npar + 1;
            auto v_N0 = p[std::pair(Npar2 + 1, Npar2 + 1)];
            
            return build<Patch_Model>(
                N_St(6),
                build<Q0>(var::build_<Matrix<double>>(
                    6, 6, {{0, 5}, {1, 0}, {2, 1}, {3, 2}, {3, 4}, {4, 3}},
                    {inactivation_rate, koff, koff * 2.0, koff * 3.0, gating_on,
                     gating_off})),
                build<Qa>(var::build_<Matrix<double>>(
                    6, 6, {{0, 1}, {1, 2}, {2, 3}}, {kon * 3.0, kon * 2.0, kon})),
                build<P_initial>(
                    var::build_<Matrix<double>>(1, 6, {{0, 0}}, {1.0})),
                build<g>(var::build_<Matrix<double>>(6, 1, {{4, 0}},
                                                     {v_unitary_current})),
                build<N_Ch_mean>(v_N0), build<Current_Noise>(v_curr_noise),
                build<Current_Baseline>(v_baseline),
                N_Ch_mean_time_segment_duration(12100),
                Binomial_magical_number(5.0), min_P(1e-7),
                Probability_error_tolerance(1e-2),
                Conductance_variance_error_tolerance(1e-2));
        },
        logp, typename Parameters<Model0>::Names(names_model),
        std::move(v_Q0_formula), std::move(v_Qa_formula),
        std::move(v_g_formula));
});

auto prior_model00_7 = Custom_Distribution(
    9ul,
    [](mt_64i &mt) {
        double kon = 10.0;
        double koff = 100;
        double gating_on = 100;
        double gating_off = 100;
        double inactivation_rate = 1e-4;
        double unitary_current = 0.5;
        double current_noise = 1e-4;
        double current_baseline = 1; // zero after log10
        double Num_ch_mean = 1000;
        double global_std_log10_par = 2.0;
        
        auto parv = std::vector<double>{kon,
                                        koff,
                                        gating_on,
                                        gating_off,
                                        inactivation_rate,
                                        unitary_current,
                                        current_noise,
                                        current_baseline,
                                        Num_ch_mean};
        
        auto n_parv = parv.size();
        
        auto par = Matrix<double>(parv.size(), 1ul, parv);
        auto logpar = apply([](auto x) { return std::log10(x); }, par);
        
        auto s_logpar = std::vector<double>(n_parv, global_std_log10_par);
        
        auto covPar = DiagPosDetMatrix<double>(s_logpar);
        
        auto distPar = make_multivariate_normal_distribution(logpar, covPar);
        
        auto sample_par = distPar.value()(mt);
        
        // now recalculate the N_ch_numbers according to segements
        
        return Parameters<Model0>(sample_par);
    },
    [](Parameters<Model0> const &logp) {
        double kon = 10.0;
        double koff = 100;
        double gating_on = 100;
        double gating_off = 100;
        double inactivation_rate = 1e-4;
        double unitary_current = 0.5;
        double current_noise = 1e-4;
        double current_baseline = 1; // zero after log10
        double Num_ch_mean = 1000;
        double global_std_log10_par = 2.0;
        
        auto parv = std::vector<double>{kon,
                                        koff,
                                        gating_on,
                                        gating_off,
                                        inactivation_rate,
                                        unitary_current,
                                        current_noise,
                                        current_baseline,
                                        Num_ch_mean};
        
        auto n_parv = parv.size();
        auto par = Matrix<double>(parv.size(), 1ul, parv);
        auto logpar = apply([](auto x) { return std::log10(x); }, par);
        
        auto s_logpar = std::vector<double>(n_parv, global_std_log10_par);
        
        auto covPar = DiagPosDetMatrix<double>(s_logpar);
        
        auto distPar = make_multivariate_normal_distribution(logpar, covPar);
        
        return distPar.value().logP(logp());
    });

auto model00 = Model0::Model([]() {
    auto names_model = std::vector<std::string>{"kon",
                                                "koff",
                                                "gatin_on",
                                                "gating_off",
                                                "inactivation_rate",
                                                "unitary_current"};
    auto names_other = std::vector<std::string>{
                                                "Current_Noise", "Current_Baseline", "Num_ch_mean", "Num_ch_stddev",
                                                "Num_ch_0",      "Num_ch_1",         "Num_ch_2",    "Num_ch_3",
                                                "Num_ch_4",      "Num_ch_5",         "Num_ch_6"};
    
    std::size_t N = 6ul;
    
    auto v_Q0_formula = Q0_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Q0_formula()[1][0] = "koff";
    v_Q0_formula()[2][1] = "2*koff";
    v_Q0_formula()[3][2] = "3*koff";
    v_Q0_formula()[3][4] = "gatin_on";
    v_Q0_formula()[4][3] = "gating_off";
    v_Q0_formula()[0][5] = "inactivation_rate";
    
    auto v_Qa_formula = Qa_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Qa_formula()[0][1] = "3*kon";
    v_Qa_formula()[1][2] = "2*kon";
    v_Qa_formula()[2][3] = "kon";
    auto v_g_formula = g_formula(std::vector<std::string>(N, ""));
    v_g_formula()[4] = "unitary_current";
    
    names_model.insert(names_model.end(), names_other.begin(),
                       names_other.end());
    auto p = Parameters<Model0>(
        std::vector<double>{10, 200, 1500, 50, 1e-5, 1, 1e-4, 1, 1000, 100,
                            1000, 1000, 1000, 1000, 1000, 1000, 1000});
    
    auto logp =
        Parameters<Model0>(apply([](auto x) { return std::log10(x); }, p()));
    
    return std::tuple(
        [](const auto &logp) {
            using std::pow;
            auto p = apply([](const auto &x) { return pow(10.0, x); }, logp());
            auto kon = p[0];
            auto koff = p[1];
            auto gating_on = p[2];
            auto gating_off = p[3];
            auto inactivation_rate = p[4];
            auto v_unitary_current = p[5] * -1.0;
            auto Npar = 6ul;
            auto v_curr_noise = p[Npar];
            auto v_baseline = logp()[Npar + 1];
            //  auto v_Num_ch_mean=p[Npar+2];
            //  auto v_std_log_Num_ch=p[Npar+3];
            
            auto Npar2 = Npar + 3;
            auto v_N0 = p[std::pair(Npar2 + 1, Npar2 + 7)];
            return build<Patch_Model>(
                N_St(6),
                build<Q0>(var::build_<Matrix<double>>(
                    6, 6, {{0, 5}, {1, 0}, {2, 1}, {3, 2}, {3, 4}, {4, 3}},
                    {inactivation_rate, koff, koff * 2.0, koff * 3.0, gating_on,
                     gating_off})),
                build<Qa>(var::build_<Matrix<double>>(
                    6, 6, {{0, 1}, {1, 2}, {2, 3}}, {kon * 3.0, kon * 2.0, kon})),
                build<P_initial>(
                    var::build_<Matrix<double>>(1, 6, {{0, 0}}, {1.0})),
                build<g>(var::build_<Matrix<double>>(6, 1, {{4, 0}},
                                                     {v_unitary_current})),
                build<N_Ch_mean>(v_N0), build<Current_Noise>(v_curr_noise),
                build<Current_Baseline>(v_baseline),
                N_Ch_mean_time_segment_duration(121),
                Binomial_magical_number(5.0), min_P(1e-7),
                Probability_error_tolerance(1e-2),
                Conductance_variance_error_tolerance(1e-2));
        },
        logp, typename Parameters<Model0>::Names(names_model),
        std::move(v_Q0_formula), std::move(v_Qa_formula),
        std::move(v_g_formula));
});

auto prior_model00 = Custom_Distribution(
    17ul,
    [](mt_64i &mt) {
        double kon = 10.0;
        double koff = 100;
        double gating_on = 100;
        double gating_off = 100;
        double inactivation_rate = 1e-4;
        double unitary_current = 0.5;
        double current_noise = 1e-4;
        double current_baseline = 1; // zero after log10
        double Num_ch_mean = 1000;
        double std_log_Num_ch = 5e-2;
        double Num_ch_factor_n = 1.0;
        std::size_t N_ch_segments = 7;
        double global_std_log10_par = 2.0;
        
        auto parv = std::vector<double>{kon,
                                        koff,
                                        gating_on,
                                        gating_off,
                                        inactivation_rate,
                                        unitary_current,
                                        current_noise,
                                        current_baseline,
                                        Num_ch_mean,
                                        std_log_Num_ch};
        
        auto n_parv = parv.size();
        
        parv.insert(parv.end(), N_ch_segments, Num_ch_factor_n);
        
        auto par = Matrix<double>(parv.size(), 1ul, parv);
        auto logpar = apply([](auto x) { return std::log10(x); }, par);
        
        auto s_logpar = std::vector<double>(n_parv - 1, global_std_log10_par);
        s_logpar.insert(s_logpar.end(), N_ch_segments + 1, 1);
        
        auto covPar = DiagPosDetMatrix<double>(s_logpar);
        
        auto distPar = make_multivariate_normal_distribution(logpar, covPar);
        
        auto sample_par = distPar.value()(mt);
        
        // now recalculate the N_ch_numbers according to segements
        
        auto s_log_Num_ch_mean = sample_par[n_parv - 2];
        auto s_std_log_Num_ch_cv = std::pow(10.0, sample_par[n_parv - 1]);
        
        for (std::size_t i = n_parv; i < par.size(); ++i)
            sample_par[i] =
                sample_par[i] / global_std_log10_par * s_std_log_Num_ch_cv +
                s_log_Num_ch_mean;
        
        return Parameters<Model0>(sample_par);
    },
    [](Parameters<Model0> const &logp) {
        double kon = 10.0;
        double koff = 100;
        double gating_on = 100;
        double gating_off = 100;
        double inactivation_rate = 1e-4;
        double unitary_current = 0.5;
        double current_noise = 1e-4;
        double current_baseline = 1; // zero after log10
        double Num_ch_mean = 1000;
        double std_logNum_ch = 5e-2;
        double Num_ch_factor_n = 1.0; //
        std::size_t N_ch_segments = 7;
        double global_std_log10_par = 2.0;
        
        auto parv = std::vector<double>{kon,
                                        koff,
                                        gating_on,
                                        gating_off,
                                        inactivation_rate,
                                        unitary_current,
                                        current_noise,
                                        current_baseline,
                                        Num_ch_mean,
                                        std_logNum_ch};
        
        auto n_parv = parv.size();
        auto p_Num_ch_mean = std::pow(10.0, logp()[n_parv - 2]);
        auto p_std_logNum_ch = std::pow(10.0, logp()[n_parv - 1]);
        
        parv.insert(parv.end(), N_ch_segments, p_Num_ch_mean);
        
        auto par = Matrix<double>(parv.size(), 1ul, parv);
        auto logpar = apply([](auto x) { return std::log10(x); }, par);
        
        auto s_logpar = std::vector<double>(n_parv, global_std_log10_par);
        s_logpar.insert(s_logpar.end(), N_ch_segments, p_std_logNum_ch);
        
        auto covPar = DiagPosDetMatrix<double>(s_logpar);
        
        auto distPar = make_multivariate_normal_distribution(logpar, covPar);
        
        return distPar.value().logP(logp());
    });

auto model01 = Model0::Model([]() {
    auto names_model = std::vector<std::string>{"kon",
                                                "koff",
                                                "gatin_on",
                                                "gating_off",
                                                "inactivation_rate",
                                                "unitary_current"};
    auto names_other =
        std::vector<std::string>{"Num_ch", "Current_Noise", "Current_Baseline"};
    
    std::size_t N = 6ul;
    
    auto v_Q0_formula = Q0_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Q0_formula()[1][0] = "koff";
    v_Q0_formula()[2][1] = "2*koff";
    v_Q0_formula()[3][2] = "3*koff";
    v_Q0_formula()[3][4] = "gatin_on";
    v_Q0_formula()[4][3] = "gating_off";
    v_Q0_formula()[0][5] = "inactivation_rate";
    
    auto v_Qa_formula = Qa_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Qa_formula()[0][1] = "3*kon";
    v_Qa_formula()[1][2] = "2*kon";
    v_Qa_formula()[2][3] = "kon";
    auto v_g_formula = g_formula(std::vector<std::string>(N, ""));
    v_g_formula()[4] = "unitary_current";
    
    names_model.insert(names_model.end(), names_other.begin(),
                       names_other.end());
    auto p = Parameters<Model0>(
        std::vector<double>{10, 200, 1500, 50, 1e-5, 1, 1000, 1e-4, 1});
    
    auto logp =
        Parameters<Model0>(apply([](auto x) { return std::log10(x); }, p()));
    
    return std::tuple(
        [](const auto &logp) {
            using std::pow;
            auto p = apply([](const auto &x) { return pow(10.0, x); }, logp());
            auto kon = p[0];
            auto koff = p[1];
            auto gating_on = p[2];
            auto gating_off = p[3];
            auto inactivation_rate = p[4];
            auto v_unitary_current = p[5] * -1.0;
            auto Npar = 6ul;
            auto v_N0 = p[Npar];
            auto v_curr_noise = p[Npar + 1];
            auto v_baseline = logp()[Npar + 2];
            return build<Patch_Model>(
                N_St(6),
                build<Q0>(var::build_<Matrix<double>>(
                    6, 6, {{0, 5}, {1, 0}, {2, 1}, {3, 2}, {3, 4}, {4, 3}},
                    {inactivation_rate, koff, koff * 2.0, koff * 3.0, gating_on,
                     gating_off})),
                build<Qa>(var::build_<Matrix<double>>(
                    6, 6, {{0, 1}, {1, 2}, {2, 3}}, {kon * 3.0, kon * 2.0, kon})),
                build<P_initial>(
                    var::build_<Matrix<double>>(1, 6, {{0, 0}}, {1.0})),
                build<g>(var::build_<Matrix<double>>(6, 1, {{4, 0}},
                                                     {v_unitary_current})),
                build<N_Ch_mean>(v_N0), build<Current_Noise>(v_curr_noise),
                build<Current_Baseline>(v_baseline), Binomial_magical_number(5.0),
                min_P(1e-7), Probability_error_tolerance(1e-2),
                Conductance_variance_error_tolerance(1e-2));
        },
        logp, typename Parameters<Model0>::Names(names_model),
        std::move(v_Q0_formula), std::move(v_Qa_formula),
        std::move(v_g_formula));
});

auto model4 = Model0::Model([]() {
    auto names_model = std::vector<std::string>{"k01",
                                                "k10",
                                                "k12",
                                                "k21",
                                                "k23",
                                                "k32",
                                                "k34",
                                                "k43",
                                                "k45",
                                                "k54",
                                                "k46",
                                                "k64",
                                                "k57",
                                                "k75",
                                                "k67",
                                                "k08",
                                                "unitary_current"};
    auto names_other =
        std::vector<std::string>{"Num_ch", "Current_Noise", "Current_Baseline"};
    
    std::size_t N = 9ul;
    
    auto v_Q0_formula = Q0_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Q0_formula()[0][8] = "k08";
    v_Q0_formula()[1][0] = "k10";
    v_Q0_formula()[2][1] = "k21";
    v_Q0_formula()[3][2] = "k32";
    v_Q0_formula()[3][4] = "k34";
    v_Q0_formula()[4][3] = "k43";
    v_Q0_formula()[4][5] = "k45";
    v_Q0_formula()[5][4] = "k54";
    v_Q0_formula()[4][6] = "k46";
    v_Q0_formula()[6][4] = "k64";
    v_Q0_formula()[5][7] = "k57";
    v_Q0_formula()[7][5] = "k75";
    v_Q0_formula()[6][7] = "k67";
    v_Q0_formula()[7][6] = "(k75 * k54 * k46 * k67)/ (k64 * k45 * k57)";
    auto v_Qa_formula = Qa_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Qa_formula()[0][1] = "k01";
    v_Qa_formula()[1][2] = "k12";
    v_Qa_formula()[2][3] = "k23";
    auto v_g_formula = g_formula(std::vector<std::string>(N, ""));
    v_g_formula()[5] = "unitary_current";
    v_g_formula()[6] = "unitary_current";
    
    names_model.insert(names_model.end(), names_other.begin(),
                       names_other.end());
    
    auto p_kinetics = std::vector<double>(16, 100.0);
    p_kinetics[0] = 10;
    p_kinetics[2] = 10;
    p_kinetics[4] = 10;
    p_kinetics[12] = 100;
    p_kinetics[15] = 1e-3;
    
    auto p_k_MH2007 = std::vector<double>{
                                          15.98, 0.019, 16.3,  380,   11.6,  6822, 3718, 43.54,
                                          540,   1088,  0.033, 0.246, 31.16, 79.0, 4.53, 1e-5};
    auto p_other = std::vector<double>{1, 4800, 1e-3, 1};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    p_k_MH2007.insert(p_k_MH2007.end(), p_other.begin(), p_other.end());
    
    //    auto p = Parameters<Model0>(p_kinetics);
    auto p = Parameters<Model0>(p_k_MH2007);
    
    auto logp =
        Parameters<Model0>(apply([](auto x) { return std::log10(x); }, p()));
    
    return std::tuple(
        [](const auto &logp) {
            using std::pow;
            auto p = apply([](const auto &x) { return pow(10.0, x); }, logp());
            auto Nst = 9ul;
            auto k01 = p[0];
            auto k10 = p[1];
            auto k12 = p[2];
            auto k21 = p[3];
            auto k23 = p[4];
            auto k32 = p[5];
            auto k34 = p[6];
            auto k43 = p[7];
            auto k45 = p[8];
            auto k54 = p[9];
            auto k46 = p[10];
            auto k64 = p[11];
            auto k57 = p[12];
            auto k75 = p[13];
            auto k67 = p[14];
            auto k76 = (k75 * k54 * k46 * k67) / (k64 * k45 * k57);
            auto k08 = p[15];
            auto v_g = p[16] * -1.0;
            auto Npar = 17ul;
            auto v_N0 = p[std::pair(Npar, Npar)];
            auto v_curr_noise = p[Npar + 1];
            auto v_baseline = logp()[Npar + 2];
            
            return build<Patch_Model>(
                N_St(Nst),
                build<Q0>(var::build_<Matrix<double>>(Nst, Nst,
                                                      {{1, 0},
                                                       {2, 1},
                                                       {3, 2},
                                                       {3, 4},
                                                       {4, 3},
                                                       {4, 5},
                                                       {5, 4},
                                                       {4, 6},
                                                       {6, 4},
                                                       {5, 7},
                                                       {7, 5},
                                                       {6, 7},
                                                       {7, 6},
                                                       {0, 8}},
                                                      {k10, k21, k32, k34, k43,
                                                       k45, k54, k46, k64, k57,
                                                       k75, k67, k76, k08})),
                build<Qa>(var::build_<Matrix<double>>(
                    Nst, Nst, {{0, 1}, {1, 2}, {2, 3}}, {k01, k12, k23})),
                build<P_initial>(
                    var::build_<Matrix<double>>(1, Nst, {{0, 0}}, {1.0})),
                build<g>(var::build_<Matrix<double>>(Nst, 1, {{5, 0}, {6, 0}},
                                                     {v_g, v_g})),
                build<N_Ch_mean>(v_N0),
                
                build<Current_Noise>(v_curr_noise),
                build<Current_Baseline>(v_baseline),
                N_Ch_mean_time_segment_duration(121000),
                Binomial_magical_number(5.0), min_P(1e-14),
                Probability_error_tolerance(1e-2),
                Conductance_variance_error_tolerance(1e-2));
        },
        std::move(logp), std::move(names_model), v_Q0_formula, v_Qa_formula,
        v_g_formula);
});


auto model4_g_lin = Model0::Model([]() {
    auto names_model = std::vector<std::string>{"k01",
                                                "k10",
                                                "k12",
                                                "k21",
                                                "k23",
                                                "k32",
                                                "k34",
                                                "k43",
                                                "k45",
                                                "k54",
                                                "k46",
                                                "k64",
                                                "k57",
                                                "k75",
                                                "k67",
                                                "k08",
                                                "unitary_current"};
    auto names_other =
        std::vector<std::string>{"Num_ch", "Current_Noise", "Current_Baseline"};
    
    std::size_t N = 9ul;
    
    auto v_Q0_formula = Q0_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Q0_formula()[0][8] = "k08";
    v_Q0_formula()[1][0] = "k10";
    v_Q0_formula()[2][1] = "k21";
    v_Q0_formula()[3][2] = "k32";
    v_Q0_formula()[3][4] = "k34";
    v_Q0_formula()[4][3] = "k43";
    v_Q0_formula()[4][5] = "k45";
    v_Q0_formula()[5][4] = "k54";
    v_Q0_formula()[4][6] = "k46";
    v_Q0_formula()[6][4] = "k64";
    v_Q0_formula()[5][7] = "k57";
    v_Q0_formula()[7][5] = "k75";
    v_Q0_formula()[6][7] = "k67";
    v_Q0_formula()[7][6] = "(k75 * k54 * k46 * k67)/ (k64 * k45 * k57)";
    auto v_Qa_formula = Qa_formula(std::vector<std::vector<std::string>>(
        N, std::vector<std::string>(N, "")));
    v_Qa_formula()[0][1] = "k01";
    v_Qa_formula()[1][2] = "k12";
    v_Qa_formula()[2][3] = "k23";
    auto v_g_formula = g_formula(std::vector<std::string>(N, ""));
    v_g_formula()[5] = "unitary_current";
    v_g_formula()[6] = "unitary_current";
    
    names_model.insert(names_model.end(), names_other.begin(),
                       names_other.end());
    
    auto p_kinetics = std::vector<double>(16, 100.0);
    p_kinetics[0] = 10;
    p_kinetics[2] = 10;
    p_kinetics[4] = 10;
    p_kinetics[12] = 100;
    p_kinetics[15] = 1e-3;
    
    auto p_other = std::vector<double>{0.1, 1000, 1e-3, 1};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    auto p = Parameters<Model0>(p_kinetics);
    
    auto logp =
        Parameters<Model0>(apply([](auto x) { return std::log10(x); }, p()));
    
    return std::tuple(
        [](const auto &logp) {
            using std::pow;
            auto p = apply([](const auto &x) { return pow(10.0, x); }, logp());
            auto Nst = 9ul;
            auto k01 = p[0];
            auto k10 = p[1];
            auto k12 = p[2];
            auto k21 = p[3];  
            auto k23 = p[4];
            auto k32 = p[5];
            auto k34 = p[6];
            auto k43 = p[7];
            auto k45 = p[8];
            auto k54 = p[9];
            auto k46 = p[10];
            auto k64 = p[11];
            auto k57 = p[12];
            auto k75 = p[13];
            auto k67 = p[14];
            auto k76 = (k75 * k54 * k46 * k67) / (k64 * k45 * k57);
            auto k08 = p[15];
            auto v_g = logp()[16];
            auto Npar = 17ul;
            auto v_N0 = p[std::pair(Npar, Npar)];
            auto v_curr_noise = p[Npar + 1];
            auto v_baseline = logp()[Npar + 2];
            
            return build<Patch_Model>(
                N_St(Nst),
                build<Q0>(var::build_<Matrix<double>>(Nst, Nst,
                                                      {{1, 0},
                                                       {2, 1},
                                                       {3, 2},
                                                       {3, 4},
                                                       {4, 3},
                                                       {4, 5},
                                                       {5, 4},
                                                       {4, 6},
                                                       {6, 4},
                                                       {5, 7},
                                                       {7, 5},
                                                       {6, 7},
                                                       {7, 6},
                                                       {0, 8}},
                                                      {k10, k21, k32, k34, k43,
                                                       k45, k54, k46, k64, k57,
                                                       k75, k67, k76, k08})),
                build<Qa>(var::build_<Matrix<double>>(
                    Nst, Nst, {{0, 1}, {1, 2}, {2, 3}}, {k01, k12, k23})),
                build<P_initial>(
                    var::build_<Matrix<double>>(1, Nst, {{0, 0}}, {1.0})),
                build<g>(var::build_<Matrix<double>>(Nst, 1, {{5, 0}, {6, 0}},
                                                     {v_g, v_g})),
                build<N_Ch_mean>(v_N0),
                
                build<Current_Noise>(v_curr_noise),
                build<Current_Baseline>(v_baseline),
                N_Ch_mean_time_segment_duration(121000),
                Binomial_magical_number(5.0), min_P(1e-7),
                Probability_error_tolerance(1e-2),
                Conductance_variance_error_tolerance(1e-2));
        },
        std::move(logp), std::move(names_model), v_Q0_formula, v_Qa_formula,
        v_g_formula);
});

auto model6 = Allost1::Model([]() {
    auto v_binding = Conformational_change_label{"Binding"};
    auto v_rocking = Conformational_change_label{"Rocking"};
    auto v_gating = Conformational_change_label{"Gating"};
    
    auto mo = make_Conformational_model(
        Agonist_dependency_map{
                               
                               std::map<Conformational_change_label, Agonist_dependency>{
                                                                                         {v_binding, Agonist_dependency{Agonist_label{"ATP"}}},
                                                                                         {v_rocking, Agonist_dependency{}},
                                                                                         {v_gating, Agonist_dependency{}}}},
        std::vector<Conformational_change_label>{
                                                 v_binding, v_binding, v_binding, v_rocking, v_gating},
        std::vector<Conformational_interaction>{
                                                {Vector_Space{
                                                                 Conformational_interaction_label{"BR"},
                                                                 Conformational_interaction_players{{v_binding, v_rocking}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 3}, {1, 3}, {2, 3}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"BG"},
                                                                 Conformational_interaction_players{{v_binding, v_gating}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 4}, {1, 4}, {2, 4}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"RG"},
                                                                 Conformational_interaction_players{{v_rocking, v_gating}},
                                                                 Conformational_interaction_positions{{{3, 4}}}}
                                                    
                                                }},
        std::vector<Conductance_interaction>{
                                             Vector_Space{Conductance_interaction_label{"Gating_Current"},
                                                          Conductance_interaction_players{{v_gating}},
                                                          Conductance_interaction_positions{{{{4}}}}}});
    
    assert(mo);
    auto m = std::move(mo.value());
    
    auto names = make_ModelNames<Allost1>(m);
    
    auto names_vec = std::vector<std::string>{
                                              "Binding_on", "Binding_off", "Rocking_on", "Rocking_off",
                                              "Gating_on",  "Gating_off",  "BR",         "BR_0",
                                              "BR_1",       "BG",          "BG_0",       "BG_1",
                                              "RG",         "RG_0",        "RG_1",       "Gating_Current"}; //--> 8
    auto names_other = std::vector<std::string>{
                                                "Inactivation_rate", "Num_ch", "Current_Noise", "Current_Baseline"};
    
    auto p_kinetics = std::vector<double>{
                                          10, 10000, 100, 10000, 1, 10000, 10, 1, 1, 10, 1, 1, 10, 1, 1, 1};
    auto p_other = std::vector<double>{1e-3, 1000, 1e-3, 1};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    auto p = Parameters<Allost1>(p_kinetics);
    
    auto logp =
        Parameters<Allost1>(apply([](auto x) { return std::log10(x); }, p()));
    
    assert(names() == names_vec);
    
    names_vec.insert(names_vec.end(), names_other.begin(), names_other.end());
    
    auto Maybe_modeltyple_formula = make_Model_Formulas<Allost1>(m, names);
    assert(Maybe_modeltyple_formula);
    auto [a_Q0_formula, a_Qa_formula, a_g_formula] =
        std::move(Maybe_modeltyple_formula.value());
    return std::tuple(
        [names, m](const auto &logp) {
            using std::pow;
            auto p = build<Parameters<Allost1>>(
                apply([](const auto &x) { return pow(10.0, x); }, logp()));
            
            p()[names["BR_0"].value()] =
                p()[names["BR_0"].value()] / (1.0 + p()[names["BR_0"].value()]);
            p()[names["BR_1"].value()] =
                p()[names["BR_1"].value()] / (1.0 + p()[names["BR_1"].value()]);
            
            p()[names["BG_0"].value()] =
                p()[names["BG_0"].value()] / (1.0 + p()[names["BG_0"].value()]);
            p()[names["BG_1"].value()] =
                p()[names["BG_1"].value()] / (1.0 + p()[names["BG_1"].value()]);
            
            p()[names["RG_0"].value()] =
                p()[names["RG_0"].value()] / (1.0 + p()[names["RG_0"].value()]);
            p()[names["RG_1"].value()] =
                p()[names["RG_1"].value()] / (1.0 + p()[names["RG_1"].value()]);
            
            p()[names["Gating_Current"].value()] =
                p()[names["Gating_Current"].value()] * -1.0;
            auto Maybe_Q0Qag = make_Model<Allost1>(m, names, p);
            assert(Maybe_Q0Qag);
            auto [a_Q0, a_Qa, a_g] = std::move(Maybe_Q0Qag.value());
            
            
            auto Npar = names().size();
            
            auto v_Inac_rate = p()[Npar];
            auto v_N0 = p()[std::pair{Npar + 1, Npar + 1}];
            auto v_curr_noise = p()[Npar + 2];
            auto v_baseline = logp()[Npar + 3];
            auto Nst = get<N_St>(m())();
            auto v_P_initial =macrodr::Macro_DMR{}.calc_Pinitial(a_Q0,a_Qa,ATP_concentration(0.0),Nst);
            return add_Patch_inactivation(
                build<Patch_Model>(N_St(get<N_St>(m())), std::move(a_Q0),
                                   std::move(a_Qa),
                                   std::move(v_P_initial),
                                   std::move(a_g), build<N_Ch_mean>(v_N0),
                                   build<Current_Noise>(v_curr_noise),
                                   build<Current_Baseline>(v_baseline),
                                   N_Ch_mean_time_segment_duration(120000),
                                   Binomial_magical_number(5.0), min_P(1e-7),
                                   Probability_error_tolerance(1e-2),
                                   Conductance_variance_error_tolerance(1e-2)),
                v_Inac_rate);
        },
        logp, names_vec, a_Q0_formula, a_Qa_formula, a_g_formula);
});

auto model6_no_inactivation = Allost1::Model([]() {
    auto v_binding = Conformational_change_label{"Binding"};
    auto v_rocking = Conformational_change_label{"Rocking"};
    auto v_gating = Conformational_change_label{"Gating"};
    
    auto mo = make_Conformational_model(
        Agonist_dependency_map{
                               
                               std::map<Conformational_change_label, Agonist_dependency>{
                                                                                         {v_binding, Agonist_dependency{Agonist_label{"ATP"}}},
                                                                                         {v_rocking, Agonist_dependency{}},
                                                                                         {v_gating, Agonist_dependency{}}}},
        std::vector<Conformational_change_label>{
                                                 v_binding, v_binding, v_binding, v_rocking, v_gating},
        std::vector<Conformational_interaction>{
                                                {Vector_Space{
                                                                 Conformational_interaction_label{"BR"},
                                                                 Conformational_interaction_players{{v_binding, v_rocking}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 3}, {1, 3}, {2, 3}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"BG"},
                                                                 Conformational_interaction_players{{v_binding, v_gating}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 4}, {1, 4}, {2, 4}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"RG"},
                                                                 Conformational_interaction_players{{v_rocking, v_gating}},
                                                                 Conformational_interaction_positions{{{3, 4}}}}
                                                    
                                                }},
        std::vector<Conductance_interaction>{
                                             Vector_Space{Conductance_interaction_label{"Gating_Current"},
                                                          Conductance_interaction_players{{v_gating}},
                                                          Conductance_interaction_positions{{{{4}}}}}});
    
    assert(mo);
    auto m = std::move(mo.value());
    
    auto names = make_ModelNames<Allost1>(m);
    
    auto names_vec = std::vector<std::string>{
                                              "Binding_on", "Binding_off", "Rocking_on", "Rocking_off",
                                              "Gating_on",  "Gating_off",  "BR",         "BR_0",
                                              "BR_1",       "BG",          "BG_0",       "BG_1",
                                              "RG",         "RG_0",        "RG_1",       "Gating_Current"}; //--> 8
    auto names_other =
        std::vector<std::string>{"Num_ch", "Current_Noise", "Current_Baseline"};
    
    auto p_kinetics = std::vector<double>{
                                          10, 1000, 1000, 100000, 1, 100, 100, 1, 1, 1, 1, 1, 100, 1, 1, 1};
    auto p_Moffatt_Hume_transformed = std::vector<double>{
                                                          9.28,     1871, 2547.88,   295207, 0.220378, 150.312, 74.865, 0.0323846,
                                                          0.187903, 1.77, -0.457748, 1,      123,      1,       1.3411, 1};
    auto p_other = std::vector<double>{1000, 1e-3, 1};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    auto p = Parameters<Allost1>(p_kinetics);
    
    auto logp =
        Parameters<Allost1>(apply([](auto x) { return std::log10(x); }, p()));
    
    assert(names() == names_vec);
    
    names_vec.insert(names_vec.end(), names_other.begin(), names_other.end());
    
    auto Maybe_modeltyple_formula = make_Model_Formulas<Allost1>(m, names);
    assert(Maybe_modeltyple_formula);
    auto [a_Q0_formula, a_Qa_formula, a_g_formula] =
        std::move(Maybe_modeltyple_formula.value());
    return std::tuple(
        [names, m](const auto &logp) {
            using std::pow;
            auto p = build<Parameters<Allost1>>(
                apply([](const auto &x) { return pow(10.0, x); }, logp()));
            
            p()[names["BR_0"].value()] =
                p()[names["BR_0"].value()] / (1.0 + p()[names["BR_0"].value()]);
            p()[names["BR_1"].value()] =
                p()[names["BR_1"].value()] / (1.0 + p()[names["BR_1"].value()]);
            
            p()[names["BG_0"].value()] =
                p()[names["BG_0"].value()] / (1.0 + p()[names["BG_0"].value()]);
            p()[names["BG_1"].value()] =
                p()[names["BG_1"].value()] / (1.0 + p()[names["BG_1"].value()]);
            
            p()[names["RG_0"].value()] =
                p()[names["RG_0"].value()] / (1.0 + p()[names["RG_0"].value()]);
            p()[names["RG_1"].value()] =
                p()[names["RG_1"].value()] / (1.0 + p()[names["RG_1"].value()]);
            
            p()[names["Gating_Current"].value()] =
                p()[names["Gating_Current"].value()] * -1.0;
            auto Maybe_Q0Qag = make_Model<Allost1>(m, names, p);
            assert(Maybe_Q0Qag);
            auto [a_Q0, a_Qa, a_g] = std::move(Maybe_Q0Qag.value());
            auto Npar = names().size();
            
            // auto v_Inac_rate = p()[Npar];
            auto v_N0 = p()[std::pair{Npar, Npar}];
            auto v_curr_noise = p()[Npar + 1];
            auto v_baseline = logp()[Npar + 2];
            auto Nst = get<N_St>(m())();
            auto v_P_initial =macrodr::Macro_DMR{}.calc_Pinitial(a_Q0,a_Qa,ATP_concentration(0.0),Nst);
            
            return build<Patch_Model>(
                N_St(get<N_St>(m())), std::move(a_Q0), std::move(a_Qa),
                std::move(v_P_initial),
                std::move(a_g), build<N_Ch_mean>(v_N0),
                build<Current_Noise>(v_curr_noise),
                build<Current_Baseline>(v_baseline),
                N_Ch_mean_time_segment_duration(120000),
                Binomial_magical_number(5.0), min_P(1e-7),
                Probability_error_tolerance(1e-2),
                Conductance_variance_error_tolerance(1e-2));
        },
        logp, names_vec, a_Q0_formula, a_Qa_formula, a_g_formula);
});

auto model6_Eff_no_inactivation = Allost1::Model([]() {
    auto v_binding = Conformational_change_label{"Binding"};
    auto v_rocking = Conformational_change_label{"Rocking"};
    auto v_gating = Conformational_change_label{"Gating"};
    
    auto mo = make_Conformational_model(
        Agonist_dependency_map{
                               
                               std::map<Conformational_change_label, Agonist_dependency>{
                                                                                         {v_binding, Agonist_dependency{Agonist_label{"ATP"}}},
                                                                                         {v_rocking, Agonist_dependency{}},
                                                                                         {v_gating, Agonist_dependency{}}}},
        std::vector<Conformational_change_label>{
                                                 v_binding, v_binding, v_binding, v_rocking, v_gating},
        std::vector<Conformational_interaction>{
                                                {Vector_Space{
                                                                 Conformational_interaction_label{"BR"},
                                                                 Conformational_interaction_players{{v_binding, v_rocking}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 3}, {1, 3}, {2, 3}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"BG"},
                                                                 Conformational_interaction_players{{v_binding, v_gating}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 4}, {1, 4}, {2, 4}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"RG"},
                                                                 Conformational_interaction_players{{v_rocking, v_gating}},
                                                                 Conformational_interaction_positions{{{3, 4}}}}
                                                    
                                                }},
        std::vector<Conductance_interaction>{
                                             Vector_Space{Conductance_interaction_label{"Gating_Current"},
                                                          Conductance_interaction_players{{v_gating}},
                                                          Conductance_interaction_positions{{{{4}}}}}});
    
    assert(mo);
    auto m = std::move(mo.value());
    
    auto names = make_ModelNames<Allost1>(m);
    
    auto names_vec_untransformed = std::vector<std::string>{
                                                            "Binding_on", "Binding_off", "Rocking_on", "Rocking_off",
                                                            "Gating_on",  "Gating_off",  "BR",         "BR_0",
                                                            "BR_1",       "BG",          "BG_0",       "BG_1",
                                                            "RG",         "RG_0",        "RG_1",       "Gating_Current"}; //--> 8
    auto names_vec = std::vector<std::string>{"Binding_Act_on",
                                              "Binding_Act_off",
                                              "Rocking_on_B",
                                              "Rocking_off_B",
                                              "Gating_on_BR",
                                              "Gating_off_BR",
                                              "BR",
                                              "BR_Bon",
                                              "BR_Ron",
                                              "BG",
                                              "BG_Bon",
                                              "BG_Gon",
                                              "RG",
                                              "RG_Ron",
                                              "RG_Gon",
                                              "Gating_Current"};
    
    auto names_other =
        std::vector<std::string>{"Num_ch", "Current_Noise", "Current_Baseline"};
    
    auto p_kinetics =
        std::vector<double>{9.28, 1871, 3875, 1.07, 914, 776, 65.1 * 1.15, 1.15,
                            33.3, 1.77, 0.77, 1.77, 123, 123, 635,         1};
    auto p_other = std::vector<double>{4800, 1e-3, 1};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    auto p = Parameters<Allost1>(p_kinetics);
    
    auto logp =
        Parameters<Allost1>(apply([](auto x) { return std::log10(x); }, p()));
    
    assert(names() == names_vec_untransformed);
    
    names_vec.insert(names_vec.end(), names_other.begin(), names_other.end());
    
    auto Maybe_modeltyple_formula = make_Model_Formulas<Allost1>(m, names);
    assert(Maybe_modeltyple_formula);
    auto [a_Q0_formula, a_Qa_formula, a_g_formula] =
        std::move(Maybe_modeltyple_formula.value());
    return std::tuple(
        [names, m](const auto &logp)->Maybe_error<Transfer_Op_to<std::decay_t<decltype(logp)>, Patch_Model>>
        {
            using std::pow;
            auto tr_p = build<Parameters<Allost1>>(
                apply([](const auto &x) { return pow(10.0, x); }, logp()));
            auto Binding_on = tr_p()[0];
            auto Binding_off = tr_p()[1];
            auto Rocking_on_B = tr_p()[2];
            auto Rocking_off_B = tr_p()[3];
            
            auto Gating_on_BR = tr_p()[4];
            auto Gating_off_BR = tr_p()[5];
            auto BR = tr_p()[6];
            auto BR_Bon = tr_p()[7];
            auto BR_Ron = tr_p()[8];
            auto BG = tr_p()[9];
            auto BG_Bon = tr_p()[10];
            auto BG_Gon = tr_p()[11];
            auto RG = tr_p()[12];
            auto RG_Ron = tr_p()[13];
            auto RG_Gon = tr_p()[14];
            auto Gating_Current = tr_p()[15] * (-1.0);
            
            auto Rocking_on = Rocking_on_B / pow(BR_Bon, 3);
            auto Rocking_off = Rocking_off_B * pow(BR / BR_Bon, 3);
            
            auto Gating_on = Gating_off_BR / pow(BG_Gon, 3) / RG_Gon;
            auto Gating_off = Gating_off_BR * pow(BG / BG_Gon, 3) * RG / RG_Gon;
            
            auto BR_0 = log(BR_Bon) / log(BR);
            auto BR_1 = log(BR_Ron) / log(BR);
            
            auto BG_0 = log(BG_Bon) / log(BG);
            auto BG_1 = log(BG_Gon) / log(BG);
            
            auto RG_0 = log(RG_Ron) / log(RG);
            auto RG_1 = log(RG_Gon) / log(RG);
            
            auto p = tr_p;
            p()[2] = Rocking_on;
            p()[3] = Rocking_off;
            
            p()[4] = Gating_on;
            p()[5] = Gating_off;
            
            p()[7] = BR_0;
            p()[8] = BR_1;
            
            p()[10] = BG_0;
            p()[11] = BG_1;
            
            p()[13] = RG_0;
            p()[14] = RG_1;
            p()[15] = Gating_Current;
            auto Maybe_Q0Qag = make_Model<Allost1>(m, names, p);
            // std::cerr<<"parameters\n"<<p();
            
            assert(Maybe_Q0Qag);
            auto [a_Q0, a_Qa, a_g] = std::move(Maybe_Q0Qag.value());
            auto Npar = names().size();
            
            // auto v_Inac_rate = p()[Npar];
            auto v_N0 = tr_p()[std::pair{Npar, Npar}];
            auto v_curr_noise = tr_p()[Npar + 1];
            auto v_baseline = logp()[Npar + 2];
            auto Nst = get<N_St>(m());
            auto Maybe_v_P_initial =macrodr::Macro_DMR{}.calc_Pinitial(a_Q0,a_Qa,ATP_concentration(0.0),Nst);
            if (!Maybe_v_P_initial)
                return Maybe_v_P_initial.error();
            else
                return build<Patch_Model>(
                    N_St(get<N_St>(m())), std::move(a_Q0), std::move(a_Qa),
                    std::move(Maybe_v_P_initial.value()),
                    std::move(a_g), build<N_Ch_mean>(v_N0),
                    build<Current_Noise>(v_curr_noise),
                    build<Current_Baseline>(v_baseline),
                    N_Ch_mean_time_segment_duration(120000),
                    Binomial_magical_number(5.0), min_P(1e-7),
                    Probability_error_tolerance(1e-2),
                    Conductance_variance_error_tolerance(1e-2));
        },
        logp, names_vec, a_Q0_formula, a_Qa_formula, a_g_formula);
});


auto model7 = Allost1::Model([]() {
    auto v_rocking = Conformational_change_label{"Rocking"};
    auto v_binding = Conformational_change_label{"Binding"};
    auto v_gating = Conformational_change_label{"Gating"};
    
    auto mo = make_Conformational_model(
        Agonist_dependency_map{
                               
                               std::map<Conformational_change_label, Agonist_dependency>{
                                                                                         {v_rocking, Agonist_dependency{}},
                                                                                         {v_binding, Agonist_dependency{Agonist_label{"ATP"}}},
                                                                                         {v_gating, Agonist_dependency{}}}},
        std::vector<Conformational_change_label>{
                                                 v_binding, v_rocking, v_binding, v_rocking, v_binding, v_rocking,
                                                 v_gating},
        std::vector<Conformational_interaction>{
                                                {Vector_Space{
                                                                 Conformational_interaction_label{"BR"},
                                                                 Conformational_interaction_players{{v_binding, v_rocking}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 1}, {2, 3}, {4, 5}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"BG"},
                                                                 Conformational_interaction_players{{v_binding, v_gating}},
                                                                 Conformational_interaction_positions{
                                                                                                      {{0, 6}, {2, 6}, {4, 6}}}},
                                                    Vector_Space{
                                                                 Conformational_interaction_label{"RG"},
                                                                 Conformational_interaction_players{{v_rocking, v_gating}},
                                                                 Conformational_interaction_positions{{{1, 6}, {3, 6}, {5, 6}}}}
                                                    
                                                }},
        std::vector<Conductance_interaction>{
                                             Vector_Space{Conductance_interaction_label{"Gating_Current"},
                                                          Conductance_interaction_players{{v_gating}},
                                                          Conductance_interaction_positions{{{{6}}}}}});
    
    assert(mo);
    auto m = std::move(mo.value());
    
    auto names = make_ModelNames<Allost1>(m);
    
    auto names_vec = std::vector<std::string>{
                                              "Binding_on", "Binding_off", "Rocking_on", "Rocking_off",
                                              "Gating_on",  "Gating_off",  "BR",         "BR_0",
                                              "BR_1",       "BG",          "BG_0",       "BG_1",
                                              "RG",         "RG_0",        "RG_1",       "Gating_Current"}; //--> 8
    auto names_other = std::vector<std::string>{
                                                "Inactivation_rate", "Num_ch", "Current_Noise", "Current_Baseline"};
    
    auto p_kinetics = std::vector<double>{
                                          10, 10000, 100, 10000, 1, 10000, 10, 1, 1, 10, 1, 1, 10, 1, 1, 1};
    auto p_other = std::vector<double>{1, 1, 100, 20, 1000};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    auto p = Parameters<Allost1>(p_kinetics);
    
    auto logp =
        Parameters<Allost1>(apply([](auto x) { return std::log10(x); }, p()));
    
    assert(names() == names_vec);
    
    names_vec.insert(names_vec.end(), names_other.begin(), names_other.end());
    
    auto Maybe_modeltyple_formula = make_Model_Formulas<Allost1>(m, names);
    assert(Maybe_modeltyple_formula);
    auto [a_Q0_formula, a_Qa_formula, a_g_formula] =
        std::move(Maybe_modeltyple_formula.value());
    return std::tuple(
        [names, m](const auto &logp) {
            using std::pow;
            auto p = build<Parameters<Allost1>>(
                apply([](const auto &x) { return pow(10.0, x); }, logp()));
            
            p()[names["BR_0"].value()] =
                p()[names["BR_0"].value()] / (1.0 + p()[names["BR_0"].value()]);
            p()[names["BR_1"].value()] =
                p()[names["BR_1"].value()] / (1.0 + p()[names["BR_1"].value()]);
            
            p()[names["BG_0"].value()] =
                p()[names["BG_0"].value()] / (1.0 + p()[names["BG_0"].value()]);
            p()[names["BG_1"].value()] =
                p()[names["BG_1"].value()] / (1.0 + p()[names["BG_1"].value()]);
            
            p()[names["RG_0"].value()] =
                p()[names["RG_0"].value()] / (1.0 + p()[names["RG_0"].value()]);
            p()[names["RG_1"].value()] =
                p()[names["RG_1"].value()] / (1.0 + p()[names["RG_1"].value()]);
            
            p()[names["Gating_Current"].value()] =
                p()[names["Gating_Current"].value()] * -1.0;
            auto Maybe_Q0Qag = make_Model<Allost1>(m, names, p);
            assert(Maybe_Q0Qag);
            auto [a_Q0, a_Qa, a_g] = std::move(Maybe_Q0Qag.value());
            auto Npar = names().size();
            
            auto v_Inac_rate = p()[Npar];
            auto v_N0 = p()[Npar + 1];
            auto v_curr_noise = p()[Npar + 2];
            auto v_baseline = logp()[Npar + 3];
            auto Nst = get<N_St>(m())();
            auto v_P_initial =macrodr::Macro_DMR{}.calc_Pinitial(a_Q0,a_Qa,ATP_concentration(0.0),Nst);
            
            return add_Patch_inactivation(
                build<Patch_Model>(N_St(get<N_St>(m())), std::move(a_Q0),
                                   std::move(a_Qa),
                                   std::move(v_P_initial),
                                   std::move(a_g), build<N_Ch_mean>(v_N0),
                                   build<Current_Noise>(v_curr_noise),
                                   build<Current_Baseline>(v_baseline),
                                   Binomial_magical_number(5.0), min_P(1e-7),
                                   Probability_error_tolerance(1e-2),
                                   Conductance_variance_error_tolerance(1e-2)),
                v_Inac_rate);
        },
        logp, names_vec, a_Q0_formula, a_Qa_formula, a_g_formula);
});

auto model8 = Allost1::Model([]() {
    auto v_binding = Conformational_change_label{"Binding"};
    auto v_rocking = Conformational_change_label{"Rocking"};
    auto mo = make_Conformational_model(
        Agonist_dependency_map{
                               std::map<Conformational_change_label, Agonist_dependency>{
                                                                                         {v_binding, Agonist_dependency{Agonist_label{"ATP"}}},
                                                                                         {v_rocking, Agonist_dependency{}}}},
        std::vector<Conformational_change_label>{
                                                 v_binding, v_rocking, v_binding, v_rocking, v_binding, v_rocking},
        std::vector<Conformational_interaction>{
                                                {Vector_Space{Conformational_interaction_label{"RBR"},
                                                              Conformational_interaction_players{
                                                                                                 {v_rocking, v_binding, v_rocking}},
                                                              Conformational_interaction_positions{{{5, 0, 1},
                                                                                                    {1, 0, 5},
                                                                                                    {1, 2, 3},
                                                                                                    {3, 2, 1},
                                                                                                    {3, 4, 5},
                                                                                                    {5, 4, 3}}}}}},
        std::vector<Conductance_interaction>{Vector_Space{
                                                          Conductance_interaction_label{"Rocking_Current_factor"},
                                                          Conductance_interaction_players{{v_rocking}},
                                                          Conductance_interaction_positions{{{{1}}, {{3}}, {{5}}}}}});
    
    assert(mo);
    auto m = std::move(mo.value());
    
    auto names = make_ModelNames<Allost1>(m);
    
    auto names_vec = std::vector<std::string>{
                                              "Binding_on",  "Binding_off", "Rocking_on",
                                              "Rocking_off", "RBR",         "RBR_0",
                                              "RBR_1",       "RBR_2",       "Rocking_Current_factor"}; //--> 8
    auto names_other =
        std::vector<std::string>{"Inactivation_rate", "Leaking_current",
                                 "Num_ch", "Current_Noise", "Current_Baseline"};
    
    auto p_kinetics =
        std::vector<double>{10, 10000, 100, 10000, 100, 1.0, 1e-2, 1.0, 100};
    auto p_other = std::vector<double>{1, 1, 100, 20, 1000, 1e-3};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    auto p = Parameters<Allost1>(p_kinetics);
    
    auto logp =
        Parameters<Allost1>(apply([](auto x) { return std::log10(x); }, p()));
    
    assert(names() == names_vec);
    
    names_vec.insert(names_vec.end(), names_other.begin(), names_other.end());
    
    auto Maybe_modeltyple_formula = make_Model_Formulas<Allost1>(m, names);
    assert(Maybe_modeltyple_formula);
    auto [a_Q0_formula, a_Qa_formula, a_g_formula] =
        std::move(Maybe_modeltyple_formula.value());
    return std::tuple(
        [names, m](const auto &logp) {
            using std::pow;
            auto p = build<Parameters<Allost1>>(
                apply([](const auto &x) { return pow(10.0, x); }, logp()));
            p()[names["RBR_0"].value()] =
                p()[names["RBR_0"].value()] / (1.0 + p()[names["RBR_0"].value()]);
            p()[names["RBR_1"].value()] =
                p()[names["RBR_1"].value()] / (1.0 + p()[names["RBR_1"].value()]);
            p()[names["RBR_2"].value()] =
                p()[names["RBR_2"].value()] / (1.0 + p()[names["RBR_2"].value()]);
            
            auto Maybe_Q0Qag = make_Model<Allost1>(m, names, p);
            assert(Maybe_Q0Qag);
            auto [a_Q0, a_Qa, a_g] = std::move(Maybe_Q0Qag.value());
            auto Npar = names().size();
            
            auto v_Inac_rate = p()[Npar];
            auto v_leaking_current = p()[Npar + 1];
            auto v_N0 = p()[Npar + 2];
            auto v_curr_noise = p()[Npar + 3];
            auto v_baseline = logp()[Npar + 4];
            
            auto v_g = build<g>(apply(
                [&v_leaking_current](const auto &x) {
                    return v_leaking_current * pow(10.0, x) * (-1.0);
                },
                a_g()));
            auto Nst = get<N_St>(m())();
            auto v_P_initial =macrodr::Macro_DMR{}.calc_Pinitial(a_Q0,a_Qa,ATP_concentration(0.0),Nst);
            
            return add_Patch_inactivation(
                build<Patch_Model>(N_St(get<N_St>(m())), std::move(a_Q0),
                                   std::move(a_Qa),std::move(v_P_initial),
                                   std::move(v_g), build<N_Ch_mean>(v_N0),
                                   build<Current_Noise>(v_curr_noise),
                                   build<Current_Baseline>(v_baseline),
                                   Binomial_magical_number(5.0), min_P(1e-7),
                                   Probability_error_tolerance(1e-2),
                                   Conductance_variance_error_tolerance(1e-2)),
                v_Inac_rate);
        },
        logp, names_vec, a_Q0_formula, a_Qa_formula, a_g_formula);
});

auto model9 = Allost1::Model([]() {
    auto v_binding = Conformational_change_label{"Binding"};
    auto v_rocking = Conformational_change_label{"Rocking"};
    auto mo = make_Conformational_model(
        Agonist_dependency_map{
                               std::map<Conformational_change_label, Agonist_dependency>{
                                                                                         {v_binding, Agonist_dependency{Agonist_label{"ATP"}}},
                                                                                         {v_rocking, Agonist_dependency{}}}},
        std::vector<Conformational_change_label>{
                                                 v_binding, v_rocking, v_binding, v_rocking, v_binding, v_rocking},
        std::vector<Conformational_interaction>{{Vector_Space{
                                                              Conformational_interaction_label{"RB"},
                                                              Conformational_interaction_players{{v_rocking, v_binding}},
                                                              Conformational_interaction_positions{
                                                                                                   {{5, 0}, {1, 0}, {1, 2}, {3, 2}, {3, 4}, {5, 4}}}}}},
        std::vector<Conductance_interaction>{Vector_Space{
                                                          Conductance_interaction_label{"Rocking_Current_factor"},
                                                          Conductance_interaction_players{{v_rocking}},
                                                          Conductance_interaction_positions{{{{1}}, {{3}}, {{5}}}}}});
    
    assert(mo);
    auto m = std::move(mo.value());
    
    auto names = make_ModelNames<Allost1>(m);
    
    auto names_vec = std::vector<std::string>{
                                              "Binding_on", "Binding_off",
                                              "Rocking_on", "Rocking_off",
                                              "RB",         "RB_0",
                                              "RB_1",       "Rocking_Current_factor"}; //--> 8
    auto names_other = std::vector<std::string>{
                                                "Inactivation_rate", "Num_ch", "Current_Noise", "Current_Baseline"};
    
    auto p_kinetics =
        std::vector<double>{10, 10000, 100, 10000, 100, 1.0, 1e-2, 100};
    auto p_other = std::vector<double>{1, 1, 100, 20, 1000, 1e-3};
    
    p_kinetics.insert(p_kinetics.end(), p_other.begin(), p_other.end());
    auto p = Parameters<Allost1>(p_kinetics);
    
    auto logp =
        Parameters<Allost1>(apply([](auto x) { return std::log10(x); }, p()));
    
    assert(names() == names_vec);
    
    names_vec.insert(names_vec.end(), names_other.begin(), names_other.end());
    
    auto Maybe_modeltyple_formula = make_Model_Formulas<Allost1>(m, names);
    assert(Maybe_modeltyple_formula);
    auto [a_Q0_formula, a_Qa_formula, a_g_formula] =
        std::move(Maybe_modeltyple_formula.value());
    return std::tuple(
        [names, m](const auto &logp) {
            using std::pow;
            auto p = build<Parameters<Allost1>>(
                apply([](const auto &x) { return pow(10.0, x); }, logp()));
            p()[names["RB_0"].value()] =
                p()[names["RB_0"].value()] / (1.0 + p()[names["RB_0"].value()]);
            p()[names["RB_1"].value()] =
                p()[names["RB_1"].value()] / (1.0 + p()[names["RB_1"].value()]);
            
            auto Maybe_Q0Qag = make_Model<Allost1>(m, names, p);
            assert(Maybe_Q0Qag);
            auto [a_Q0, a_Qa, a_g] = std::move(Maybe_Q0Qag.value());
            auto Npar = names().size();
            
            auto v_Inac_rate = p()[Npar];
            auto v_leaking_current = p()[Npar + 1];
            auto v_N0 = p()[Npar + 2];
            auto v_curr_noise = p()[Npar + 3];
            auto v_baseline = logp()[Npar + 4];
            
            auto v_g = build<g>(apply(
                [&v_leaking_current](const auto &x) {
                    return v_leaking_current * pow(10.0, x) * (-1.0);
                },
                a_g()));
            auto Nst = get<N_St>(m())();
            auto v_P_initial =macrodr::Macro_DMR{}.calc_Pinitial(a_Q0,a_Qa,ATP_concentration(0.0),Nst);
            
            return add_Patch_inactivation(
                build<Patch_Model>(N_St(get<N_St>(m())), std::move(a_Q0),
                                   std::move(a_Qa),
                                   std::move(v_P_initial),
                                   std::move(v_g), build<N_Ch_mean>(v_N0),
                                   build<Current_Noise>(v_curr_noise),
                                   build<Current_Baseline>(v_baseline),
                                   Binomial_magical_number(5.0), min_P(1e-7),
                                   Probability_error_tolerance(1e-2),
                                   Conductance_variance_error_tolerance(1e-2)),
                v_Inac_rate);
        },
        logp, names_vec, a_Q0_formula, a_Qa_formula, a_g_formula);
});
} // namespace macrodr ;

#endif // MODELS_H