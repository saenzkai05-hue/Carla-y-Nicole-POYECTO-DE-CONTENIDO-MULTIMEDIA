#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <set>
#include <sstream>

#include "production.h"
#include "fileio.h"
#include "utils.h"
#include "report.h"

using namespace std;

void showMenu() {
    cout << "\n==================== PROYECTO FINAL ====================\n";
    cout << "1. Registrar nuevo contenido\n";
    cout << "2. Mostrar todos los contenidos\n";
    cout << "3. Modificar contenido\n";
    cout << "4. Eliminar contenido\n";
    cout << "5. Buscar/Filtrar contenidos\n";
    cout << "6. Guardar datos en archivo (CSV)\n";
    cout << "7. Recuperar datos de archivo (CSV)\n";
    cout << "8. Mostrar alertas del sistema\n";
    cout << "9. Intercalar dos archivos ordenados (merge)\n";
    cout << "10. Ordenar por ID (Bubble Sort optimizado)\n";
    cout << "11. Buscar por ID (secuencial/binaria)\n";
    cout << "12. Ordenar por fecha de entrega\n";
    cout << "13. Comparar eficiencia: Bubble Sort vs Merge (en memoria)\n";
    cout << "14. Generar reporte HTML (exportable a PDF)\n";
    cout << "15. Salir\n";
    cout << "========================================================\n";
    cout << "Seleccione una opcion: ";
}

Production::ResultadoComparacion mergeVectorsInMemory(const vector<Production::Content>& A_in,
                                                     const vector<Production::Content>& B_in,
                                                     vector<Production::Content>& out) {
    Production::ResultadoComparacion r; r.comparaciones = 0; r.intercambios = 0; r.tiempo_ms = 0;
    clock_t t0 = clock();

    vector<Production::Content> A = A_in;
    vector<Production::Content> B = B_in;
    out.clear();

    size_t i = 0, j = 0;
    set<int> seen;

    while (i < A.size() && j < B.size()) {
        r.comparaciones++;
        const Production::Content &ca = A[i];
        const Production::Content &cb = B[j];
        if (ca.id < cb.id) {
            if (!seen.count(ca.id)) { out.push_back(ca); seen.insert(ca.id); r.intercambios++; }
            ++i;
        } else if (cb.id < ca.id) {
            if (!seen.count(cb.id)) { out.push_back(cb); seen.insert(cb.id); r.intercambios++; }
            ++j;
        } else {
            if (!seen.count(ca.id)) { out.push_back(ca); seen.insert(ca.id); r.intercambios++; }
            ++i; ++j;
        }
    }
    while (i < A.size()) {
        r.comparaciones++;
        if (!seen.count(A[i].id)) { out.push_back(A[i]); seen.insert(A[i].id); r.intercambios++; }
        ++i;
    }
    while (j < B.size()) {
        r.comparaciones++;
        if (!seen.count(B[j].id)) { out.push_back(B[j]); seen.insert(B[j].id); r.intercambios++; }
        ++j;
    }

    clock_t t1 = clock();
    r.tiempo_ms = (long long)((t1 - t0) * 1000 / CLOCKS_PER_SEC);
    return r;
}

void compareBubbleVsMergeInMemory() {
    vector<Production::Content> original = Production::produccion;

    Production::produccion = original;
    Production::ResultadoComparacion resBubble = Production::bubbleSortByIdOptimized();

    vector<Production::Content> A, B;
    for (size_t k = 0; k < original.size(); ++k) {
        if (k % 2 == 0) A.push_back(original[k]);
        else B.push_back(original[k]);
    }

    // sort A and B by id
    for (size_t i = 0; i + 1 < A.size(); ++i) {
        for (size_t j = i + 1; j < A.size(); ++j) {
            if (A[j].id < A[i].id) {
                Production::Content tmp = A[i]; A[i] = A[j]; A[j] = tmp;
            }
        }
    }
    for (size_t i = 0; i + 1 < B.size(); ++i) {
        for (size_t j = i + 1; j < B.size(); ++j) {
            if (B[j].id < B[i].id) {
                Production::Content tmp = B[i]; B[i] = B[j]; B[j] = tmp;
            }
        }
    }

    vector<Production::Content> merged;
    Production::ResultadoComparacion resMerge = mergeVectorsInMemory(A, B, merged);

    Utils::printLine("=== RESULTADOS COMPARACION ===");
    std::stringstream ss1; ss1 << "Bubble Sort - Comparaciones: " << resBubble.comparaciones << " Intercambios: " << resBubble.intercambios << " Tiempo(ms): " << resBubble.tiempo_ms;
    Utils::printLine(ss1.str());
    std::stringstream ss2; ss2 << "Merge (mem) - Comparaciones: " << resMerge.comparaciones << " Intercambios: " << resMerge.intercambios << " Tiempo(ms): " << resMerge.tiempo_ms;
    Utils::printLine(ss2.str());

    if (resBubble.comparaciones < resMerge.comparaciones) Utils::printLine("[WIN] Bubble Sort hizo menos comparaciones.");
    else if (resMerge.comparaciones < resBubble.comparaciones) Utils::printLine("[WIN] Merge hizo menos comparaciones.");
    else {
        if (resBubble.tiempo_ms < resMerge.tiempo_ms) Utils::printLine("[WIN] Bubble Sort empató en comparaciones pero fue más rápido.");
        else if (resMerge.tiempo_ms < resBubble.tiempo_ms) Utils::printLine("[WIN] Merge empató en comparaciones pero fue más rápido.");
        else Utils::printLine("[EMPATE] Mismo número de comparaciones y tiempo similar.");
    }

    Production::produccion = original;
}

int main() {
    Production::loadSampleData();
    string input;
    while (1) {
        showMenu();
        getline(cin, input);

        if (input == "1") {
            Production::Content c = Production::makeContentFromInput();
            Production::addContent(c);
            Utils::pause();

        } else if (input == "2") {
            Production::showAll();
            Utils::pause();

        } else if (input == "3") {
            cout << "Ingrese ID a modificar: ";
            int id;
            if (!(cin >> id)) { cin.clear(); cin.ignore(10000,'\n'); Utils::printLine("[X] ID invalido."); Utils::pause(); continue; }
            cin.ignore(10000,'\n');
            if (!Production::modifyContent(id)) Utils::printLine("[X] No se encontro ID.");
            Utils::pause();

        } else if (input == "4") {
            cout << "Ingrese ID a eliminar: ";
            int id;
            if (!(cin >> id)) { cin.clear(); cin.ignore(10000,'\n'); Utils::printLine("[X] ID invalido."); Utils::pause(); continue; }
            cin.ignore(10000,'\n');
            cout << "Confirma eliminacion (S/N): ";
            string c; getline(cin, c);
            if (c == "S" || c == "s") {
                if (!Production::deleteContent(id)) Utils::printLine("[X] No se encontro ID.");
            } else Utils::printLine("[X] Eliminacion cancelada.");
            Utils::pause();

        } else if (input == "5") {
            cout << "1.Tipo 2.Cliente 3.Estado 4.Fecha\nSeleccione: ";
            string opt; getline(cin, opt);
            vector<Production::Content> res;
            if (opt == "1") { cout << "Tipo: "; string t; getline(cin,t); res = Production::filterByTipo(t); }
            else if (opt == "2") { cout << "Cliente: "; string t; getline(cin,t); res = Production::filterByCliente(t); }
            else if (opt == "3") { cout << "Estado: "; string t; getline(cin,t); res = Production::filterByEstado(t); }
            else if (opt == "4") { cout << "Fecha (YYYY-MM-DD): "; string t; getline(cin,t); res = Production::filterByFecha(t); }
            else { Utils::printLine("[X] Opcion invalida."); Utils::pause(); continue; }

            if (res.empty()) Utils::printLine("[!] No se encontraron resultados.");
            else {
                Utils::printTableHeader();
                for (size_t i = 0; i < res.size(); ++i) Utils::printContentRow(res[i]);
                std::stringstream ss; ss << "Total resultados: " << res.size();
                Utils::printLine(ss.str());
            }
            Utils::pause();

        } else if (input == "6") {
            cout << "Nombre archivo (ej: produccion.csv): ";
            string fname; getline(cin, fname);
            if (fname.empty()) fname = "produccion_multimedia.csv";
            FileIO::saveToCSV(fname);
            Utils::pause();

        } else if (input == "7") {
            cout << "Nombre archivo a recuperar: ";
            string fname; getline(cin, fname);
            if (fname.empty()) fname = "produccion_multimedia.csv";
            FileIO::loadFromCSV(fname);
            Utils::pause();

        } else if (input == "8") {
            vector< pair<Production::Content, vector<string> > > alertas = Production::detectAlerts();
            if (alertas.empty()) Utils::printLine("[OK] No hay alertas.");
            else {
                int i = 1;
                for (size_t k = 0; k < alertas.size(); ++k) {
                    Utils::printLine("ALERTA #" + Utils::intToString(i++));
                    Utils::printLine("Motivos: ");
                    for (size_t m = 0; m < alertas[k].second.size(); ++m) cout << " - " << alertas[k].second[m] << "\n";
                    Utils::printTableHeader();
                    Utils::printContentRow(alertas[k].first);
                    cout << "\n";
                }
            }
            Utils::pause();

        } else if (input == "9") {
            cout << "Archivo A (ordenado): "; string a; getline(cin,a);
            cout << "Archivo B (ordenado): "; string b; getline(cin,b);
            cout << "Archivo salida: "; string out; getline(cin,out);
            if (a.empty() || b.empty() || out.empty()) { Utils::printLine("[X] Nombres invalidos."); Utils::pause(); continue; }
            FileIO::mergeSortedFiles(a,b,out);
            Utils::pause();

        } else if (input == "10") {
            Production::ResultadoComparacion res = Production::bubbleSortByIdOptimized();
            std::stringstream ss; ss << "Comparaciones realizadas: " << res.comparaciones << " Intercambios: " << res.intercambios << " Tiempo(ms): " << res.tiempo_ms;
            Utils::printLine(ss.str());
            Utils::pause();

        } else if (input == "11") {
            cout << "Ingrese ID a buscar: ";
            int id;
            if (!(cin >> id)) { cin.clear(); cin.ignore(10000,'\n'); Utils::printLine("[X] ID invalido."); Utils::pause(); continue; }
            cin.ignore(10000,'\n');
            cout << "Metodo: 1.Secuencial 2.Binaria: ";
            string m; getline(cin, m);

            Production::ResultadoBusqueda resultado;
            if (m == "1") resultado = Production::sequentialSearchById(id);
            else resultado = Production::binarySearchById(id);

            if (resultado.indice == -1) {
                Utils::printLine("[!] No se encontro registro.");
            } else {
                Utils::printTableHeader();
                Utils::printContentRow(Production::produccion[resultado.indice]);
                Utils::printLine("Comparaciones realizadas: " + Utils::intToString(resultado.comparaciones));
            }
            Utils::pause();

        } else if (input == "12") {
            int comparaciones = 0;
            Production::sortByFechaEntrega(comparaciones);
            Utils::printLine("Comparaciones realizadas: " + Utils::intToString(comparaciones));
            Utils::pause();

        } else if (input == "15") {
            Utils::printLine("Gracias por usar el sistema. Hasta pronto!");
            break;

        } else if (input == "13") {
            compareBubbleVsMergeInMemory();
            Utils::pause();

        } else if (input == "14") {
            cout << "Nombre archivo de salida (ej: reporte_produccion.html): ";
            string fname; getline(cin, fname);
            if (fname.empty()) fname = "reporte_produccion.html";
            bool ok = Report::generateHtmlReport(fname);
            if (!ok) Utils::printLine("[X] No se pudo generar el reporte.");
            Utils::pause();

        } else {
            Utils::printLine("[X] Opcion invalida.");
            Utils::pause();
        }
    }
    return 0;
}
