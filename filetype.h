#ifndef FILETYPE_H
#define FILETYPE_H


class filetype
{
    public:
        filetype(std::string);
        virtual ~filetype();
        inline std::string comptype() { return m_compressionType; }
        inline structuredLogger & elog() { return structuredLogger::getLogger("populateDB");}
    protected:
        std::string m_filespec;
        std::string m_compressionType;
    private:
};

#endif // FILETYPE_H
