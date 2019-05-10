
// Product   GPU-dpi

/// \author  KMS - Martin Dubois, ing.
/// \file    Includes/GPU_dpi/System.h
/// \brief   GPU_dpi::System (SDK)

#pragma once

// Includes
/////////////////////////////////////////////////////////////////////////////

// ===== Includes ===========================================================
#include <GPU_dpi/Status.h>
#include <GPU_dpi/Types.h>

namespace GPU_dpi
{

    // Class
    /////////////////////////////////////////////////////////////////////////

    /// \cond en
    /// \brief  This class define the system level interface.
    /// \endcond
    /// \cond fr
    /// \brief  Cette classe definit l'interface au niveau systeme.
    /// \endcond
    class System
    {

    public:

        /// \cond en
        /// \brief   This static methode create an instance of the System
        ///          class.
        /// \return  This static methode return the new instance address.
        /// \endcond
        /// \cond fr
        /// \brief   Cette m&eacute;thode statique cr&eacute;e une instance
        ///          de la classe System.
        /// \return  Cette m&eacute;thode statique retourne l'adresse de la
        ///          nouvelle instance.
        /// \endcond
        /// \sa      Delete
        static GPU_DPI_PUBLIC System * Create();

        /// \cond en
        /// \brief   This methode delete the instance.
        /// \endcond
        /// \cond fr
        /// \brief   Cette m&eacute;thode detruit l'instance.
        /// \endcond
        /// \sa      Create
        virtual void Delete();

        /// \cond en
        /// \brief  Display statistics
        /// \param  aIndex  The adapter index
        /// \param  aOut    The output stream
        /// \endcond
        /// \cond fr
        /// \brief  Afficher les statistiques
        /// \param  aIndex  L'index de l'adaptateur
        /// \retval aOut    Le fichier de sortie
        /// \endcond
        /// \retval STATUS_OK
        virtual Status Adapter_DisplayStatistics(unsigned int aIndex, FILE * aOut) const = 0;

        /// \cond en
        /// \return  This methode return the number of adapters.
        /// \endcond
        /// \cond fr
        /// \return  Cette m&eacute;thode retourne le nombre d'adaptateurs.
        /// \endcond
        virtual unsigned int Adapter_GetCount() const = 0;

        /// \cond en
        /// \return  This methode return the configuration of an adapter.
        /// \param   aIndex   The adapter index
        /// \param   aConfig  The output buffer
        /// \endcond
        /// \cond fr
        /// \return  Cette m&eacute;thode retourne la configuration d'un
        ///          adaptateurs.
        /// \param   aIndex   L'index de l'adaptateur
        /// \param   aConfig  L'espace de sortie
        /// \endcond
        /// \retval  STATUS_OK
        virtual Status Adapter_GetConfig(unsigned int aIndex, AdapterConfig * aConfig) const = 0;

        /// \cond en
        /// \return  This methode return the information about an adapter.
        /// \param   aIndex  The adapter index
        /// \param   aInfo   The output buffer
        /// \endcond
        /// \cond fr
        /// \return  Cette m&eacute;thode retourne l'information au sujet
        ///          d'un adaptateurs.
        /// \param   aIndex  L'index de l'adaptateur
        /// \param   aInfo   L'espace de sortie
        /// \endcond
        /// \retval  STATUS_OK
        virtual Status Adapter_GetInfo(unsigned int aIndex, AdapterInfo * aInfo) const = 0;

        /// \cond en
        /// \return  This methode reset the configuration of an adapter.
        /// \param   aIndex  The adapter index
        /// \endcond
        /// \cond fr
        /// \return  Cette m&eacute;thode r&eacute;initialise la
        ///          configuration d'un adaptateurs.
        /// \param   aIndex  L'index de l'adaptateur
        /// \endcond
        /// \retval  STATUS_OK
        virtual Status Adapter_ResetConfig(unsigned int aIndex) = 0;

        /// \cond en
        /// \return  This methode modify the configuration of an adapter.
        /// \param   aIndex   The adapter index
        /// \param   aConfig  The configuration
        /// \endcond
        /// \cond fr
        /// \return  Cette m&eacute;thode change la configuration d'un
        ///          adaptateurs.
        /// \param   aIndex   L'index de l'adaptateur
        /// \param   aConfig  La configuration
        /// \endcond
        /// \retval  STATUS_OK
        virtual Status Adapter_SetConfig(unsigned int aIndex, const AdapterConfig & aConfig) = 0;

        /// \cond en
        /// \return  This methode read a configuration file.
        /// \param   aFileName  The name of the file to read
        /// \param   aLine      If an error occure, the error line is returner here
        /// \endcond
        /// \cond fr
        /// \return  Cette m&eacute;thode lit un fichier de configuration.
        /// \param   aFileName  Le nom du fichier &agrave; lire
        /// \param   aLine      Si une erreur survient, le num&eacute;ro de
        ///                     la ligne est retourn&eacute; ici.
        /// \endcond
        /// \retval  STATUS_OK
        virtual Status ConfigFile_Read(const char * aFileName, unsigned int * aLine = NULL) = 0;

        /// \cond en
        /// \brief  Display
        /// \param  aOut  The output stream
        /// \endcond
        /// \cond fr
        /// \brief  Afficher
        /// \retval aOut  Le fichier de sortie
        /// \endcond
        /// \retval STATUS_OK
        virtual Status Display(FILE * aOut) const = 0;

        /// \cond en
        /// \brief  Display statistics
        /// \param  aOut  The output stream
        /// \endcond
        /// \cond fr
        /// \brief  Afficher les statistiques
        /// \retval aOut  Le fichier de sortie
        /// \endcond
        /// \retval STATUS_OK
        virtual Status DisplayStatistics(FILE * aOut) const = 0;

        /// \cond en
        /// \return  This methode return the number of processors.
        /// \endcond
        /// \cond fr
        /// \return  Cette methode retourne le nombre de processeurs.
        /// \endcond
        virtual unsigned int Processor_GetCount() const = 0;

        /// \cond en
        /// \brief  Start
        /// \endcond
        /// \cond fr
        /// \brief  Demarrer
        /// \endcond
        /// \retval STATUS_OK
        /// \sa     Stop, START_FLAG_LOOPBACK
        virtual Status Start() = 0;

        /// \cond en
        /// \brief  Stop
        /// \endcond
        /// \cond fr
        /// \brief  Arreter
        /// \endcond
        /// \retval STATUS_OK
        /// \sa     Start
        virtual Status Stop() = 0;

        /// \cond en
        /// \brief  Wait
        /// \endcond
        /// \cond fr
        /// \brief  Attendre
        /// \endcond
        /// \retval STATUS_OK
        /// \sa     Start
        virtual Status Wait() = 0;

        /// \cond en
        /// \brief   This method writes the build log into a file.
        /// \param   aOut  The file to write to
        /// \endcond
        /// \cond fr
        /// \brief   Cette m&eacute;thode &eacute;crit le journal de
        ///          compilation dans un fichier.
        /// \param   aOut  Le fichier de sortie
        /// \endcond
        /// \retval  STATUS_OK
        virtual Status WriteBuildLog(FILE * aOut) = 0;

    protected:

        System();

        virtual ~System();

    private:

        System(const System &);

        const System & operator = (const System &);

    };

}
