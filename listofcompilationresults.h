#pragma once

///
/// \brief The CompilationResults enum - перечисление результата работы подсистемы сборки и запуска
///
enum class CompilationResults {
    Success,
    CompilerError,
    CompilationError,
    RunningError,
    EmptyTestResults
};
