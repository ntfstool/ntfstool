import ar from './ar'
import bn from './bn'
import de from './de'
import en from './en'
import es from './es'
import fr from './fr'
import hi from './hi'
import id from './id'
import it from './it'
import ko from './ko'
import pt from './pt'
import fa from './fa'
import ru from './ru'
import sw from './sw'
import th from './th'
import tr from './tr'
import vi from './vi'
import zhCN from './zhCN'
import zhTW from './zhTW'
import ja from './ja'

const languages_select = {
    languages: [
        { text: "Arabic", val: 'ar' },
        { text: "Bengali", val: 'bn' },
        { text: "Deutsch", val: 'de' },
        { text: "English", val: 'en' },
        { text: "Espanol", val: 'es' },
        { text: "French", val: 'fr' },
        { text: "Hindi", val: 'hi' },
        { text: "Indonesian", val: 'id' },
        { text: "Italian", val: 'it' },
        { text: "Korea", val: 'ko' },
        { text: "Portuguese", val: 'pt' },
        { text: "Persian", val: 'fa' },
        { text: "Русский", val: 'ru' },
        { text: "Swahili", val: 'sw' },
        { text: "Thai", val: 'th' },
        { text: "Turkish", val: 'tr' },
        { text: "Vietnamese", val: 'vi' },
        { text: "中文 (简体)", val: 'zhCN' },
        { text: "中文 (繁體)", val: 'zhTW' },
        { text: "日本語", val: 'ja' }
    ]
};

export const messages = {
    ar: { ...ar, ...languages_select },
    bn: { ...bn, ...languages_select },
    de: { ...de, ...languages_select },
    en: { ...en, ...languages_select },
    es: { ...es, ...languages_select },
    fr: { ...fr, ...languages_select },
    hi: { ...hi, ...languages_select },
    id: { ...id, ...languages_select },
    it: { ...it, ...languages_select },
    ko: { ...ko, ...languages_select },
    pt: { ...pt, ...languages_select },
    fa: { ...fa, ...languages_select },
    ru: { ...ru, ...languages_select },
    sw: { ...sw, ...languages_select },
    th: { ...th, ...languages_select },
    tr: { ...tr, ...languages_select },
    vi: { ...vi, ...languages_select },
    sw: { ...sw, ...languages_select },
    zhCN: { ...zhCN, ...languages_select },
    zhTW: { ...zhTW, ...languages_select },
    ja: { ...ja, ...languages_select },
};


export  function matchLanguage() {
    const userLanguage = navigator.language || navigator.userLanguage;
  

    const formattedUserLanguage = userLanguage.replace('-', '');
  
    const matchedLanguage = languages_select.languages.find((lang) =>
      formattedUserLanguage.toLowerCase().includes(lang.val.toLowerCase())
    );
  
    return matchedLanguage ? matchedLanguage.val : 'en';
  }