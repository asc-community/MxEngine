#include "Profiler.h"

namespace MxEngine
{
    void ProfileSession::WriteJsonHeader()
    {
        stream << "{\n";
        stream << "  \"traceEvents\": [\n";
    }

    void ProfileSession::WriteJsonFooter()
    {
        stream << "\n  ]\n";
        stream << '}';
    }

    bool ProfileSession::IsValid() const
    {
        return this->stream.is_open();
    }

    size_t ProfileSession::GetEntryCount() const
    {
        return this->count;
    }

    void ProfileSession::StartSession(const std::string& filename)
    {
        if (stream.is_open()) stream.close();
        stream.open(filename);
        this->WriteJsonHeader();
    }

    void ProfileSession::WriteJsonEntry(const char* function, TimeStep begin, TimeStep delta)
    {
        assert(this->IsValid());
        if (this->GetEntryCount() > 0)
        {
            stream << ",\n";
        }
        this->count++;

        stream << "    {";
        stream << "\"pid\": 0, ";
        stream << "\"tid\": 0, ";
        stream << "\"ts\": " << std::to_string(uint64_t((double)begin * 1000000)) << ", ";
        stream << "\"dur\": " << std::to_string(uint64_t((double)delta * 1000000)) << ", ";
        stream << "\"ph\": \"X\", ";
        stream << "\"name\": \"" << function << '\"';
        stream << "}";
    }

    void ProfileSession::EndSession()
    {
        this->WriteJsonFooter();
        stream.close();
    }
}