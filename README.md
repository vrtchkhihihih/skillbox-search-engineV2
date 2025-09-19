# skillbox-search-engineV2
Локальный поисковый движок по набору текстовых файлов с ранжированием результатов и JSON-интерфейсом конфигурации/запросов/ответов.

## Возможности
- Чтение конфигурации `config.json` (название, версия, max_responses, список путей к файлам).
- Индексация документов.
- Обработка запросов из `requests.json`, пересечение по словам, выдача наиболее релевантных документов.
- Подсчёт абсолютной и относительной релевантности; сортировка по убыванию ранга.
- Запись результатов в `answers.json` в валидном формате.
- Набор модульных тестов на **GoogleTest**.

## Требования
- **CMake ≥ 3.20**
- Компилятор **C++17** (MinGW, MSVC, Clang, GCC)
- Интернет-доступ на этапе сборки для загрузки зависимостей (`nlohmann/json`, GoogleTest).

## Сборка (CLI)
```bash
git clone <this repo>
cd search_engine_cpp_fixed
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

## Запуск приложения
Из корня репозитория (чтобы относительные пути к ресурсам работали):
```bash
./build/app/search_engine
# Windows:
.\build\app\Release\search_engine.exe
```

## Структура JSON

**config.json**
```json
{
  "config": {
    "name": "SkillboxSearchEngine",
    "version": "1.0.0",
    "max_responses": 5
  },
  "files": [
    "resources/file001.txt",
    "resources/file002.txt",
    "resources/file003.txt"
  ]
}
```

**requests.json**
```json
[
  "milk water",
  "london capital"
]
```

**answers.json** (пример результата)
```json
{
  "answers": {
    "request001": { "result": false },
    "request002": {
      "result": true,
      "relevance": [
        { "docid": 2, "rank": 1.0 }
      ]
    }
  }
}
```

В ТЗ пример `answers.json` использует повторяющиеся ключи внутри объекта, что невалидно по JSON.  
Здесь применён **массив объектов** `{docid, rank}` — валидно и удобно для парсинга.

## Структура проекта
```
search_engine_cpp_fixed/
├─ CMakeLists.txt         # головной
├─ app/                   # CLI-приложение
│  ├─ main.cpp
│  └─ CMakeLists.txt
├─ src/                   # библиотека searchlib
│  ├─ include/searchlib/  # заголовки
│  │   ├─ ConverterJSON.hpp
│  │   ├─ InvertedIndex.hpp
│  │   └─ SearchServer.hpp
│  ├─ lib/                # реализации
│  │   ├─ ConverterJSON.cpp
│  │   ├─ InvertedIndex.cpp
│  │   └─ SearchServer.cpp
│  └─ CMakeLists.txt
├─ tests/                 # модульные тесты
│  ├─ basic_tests.cpp
│  └─ CMakeLists.txt
├─ resources/             # примеры документов
│  ├─ file001.txt
│  ├─ file002.txt
│  └─ file003.txt
├─ config.json
└─ requests.json
```

## Как это работает (коротко)
- **ConverterJSON** читает `config.json`, загружает файлы, нормализует текст (латиница, нижний регистр, ≤1000 слов/файл).
- **InvertedIndex** подсчитывает частоты слов по документам и собирает частотный словарь.
- **SearchServer** разбивает запрос на уникальные слова, пересекает множество документов и считает абсолютную/относительную релевантность.
- **ConverterJSON::PutAnswers** сохраняет ответ в `answers.json`, отдавая не более `max_responses` записей на запрос.
