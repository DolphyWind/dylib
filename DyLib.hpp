/**
 * \file DyLib.hpp
 * \brief Dynamic Library Loader
 * \author Martin Olivier
 * \version 0.1
 * 
 * MIT License
 * Copyright (c) 2021 Martin Olivier
 */

#pragma once

#include <string>
#include <functional>
#include <exception>
#include <dlfcn.h>

class DyLibException : std::exception
{
protected:
    const std::string m_error;
public:        
    DyLibException(const std::string &error) : m_error(error) {};
    const char *what() const noexcept override {return m_error.c_str();};
};

class DyLib
{
private:
    void *m_handle{nullptr};
public:
/** Creates a Dynamic Library object.
 */
    DyLib() = default;
/**
 *  Creates a Dynamic Library instance.
 *
 *  @param path path to the dynamic library to load (.so, .dylib)
 *  @param mode dl lib flag -> RTLD_NOW by default
 */
    DyLib(const std::string &path, int mode = RTLD_NOW)
    {
        this->load(path, mode);
    }

    ~DyLib()
    {
        this->close();
    }

/**
 *  Load a Dynamic Library into the object. 
 *  If a Dynamic Library was Already opened, it will be unload and replaced
 *
 *  @param path path to the dynamic library to load (.so, .dylib)
 *  @param mode dl lib flag -> RTLD_NOW by default
 */
    void load(const std::string &path, int mode = RTLD_NOW)
    {
        this->close();
        m_handle = dlopen(path.c_str(), mode);
        if (!m_handle)
            throw DyLibException(dlerror());
    }

/**
 *  Get a function from the Dynamic Library inside the object.
 *
 *  @param template_Ret the first template arg must be the return value of the function
 *  @param template_Args next template arguments must be the arguments of the function
 *  @param name function to get from the Dynamic Library
 *
 *  @returns std::function<Ret, ...Args> that contains the symbol
 */
    template<class Ret, class ...Args>
    std::function<Ret(Args...)> getFunction(const std::string &name)
    {
        if (!m_handle)
            throw DyLibException(dlerror());
        void *sym = dlsym(m_handle, name.c_str());
        if (!sym)
            throw DyLibException(dlerror());
        return ((Ret (*)(Args...)) sym);
    }

/** Close the Dynamic Library currently loaded into the object.
 */
    void close() noexcept
    {
        if (m_handle)
            dlclose(m_handle);
        m_handle = nullptr;
    }
};